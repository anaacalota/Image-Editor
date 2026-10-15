Copyright 2024 Ana-Cristina Calota

# Image Editor

## Overview

A command-line image editor written in C. It loads PPM and PGM images (ASCII
and binary), keeps them in dynamically allocated memory and applies a set of
transformations on a selected region: selection, crop, rotation, image kernels
(edge detection, sharpen, blur, gaussian blur), histogram and
equalization. The result can be saved again in ASCII or binary format.

Commands: `LOAD <file>`, `SELECT <x1> <y1> <x2> <y2>`, `SELECT ALL`,
`HISTOGRAM <x> <y>`, `EQUALIZE`, `ROTATE <angle>`, `CROP`,
`APPLY <EDGE|SHARPEN|BLUR|GAUSSIAN_BLUR>`, `SAVE <file> [ascii]`, `EXIT`.
Any other input prints `Invalid command`.

## Implementation Details

* **Data representation:** `struct.h` defines two structures: `pixel`
  (`unsigned char` matrices, used only while reading a file) and `pixel_dbl`
  (`double` matrices, used for everything else). Each color channel is a
  separate matrix (`r`, `g`, `b`); black and white and grayscale images only use
  `r`. The image type is stored as 0 (black and white), 1 (grayscale) or 2
  (color). The selection is kept as two inclusive corners `(x1, y1)` and
  `(x2, y2)`. All calculations are done in `double` and the values are rounded
  only when the image is saved.
* **main.c:** reads the commands and calls one `handle_*` function for each of
  them. These functions first check that an image is loaded (otherwise they print
  `No image loaded` and discard the rest of the line) and then call the module
  that implements the command.
* **LOAD (load.c):** `comments()` skips the lines starting with `#`,
  `read_dimensions()` reads the header and the magic word gives the image type
  and the file type (ASCII or binary). Pixels are read with
  `read_ascii_matrix()` or `read_binary_matrix()` (the file is reopened in `"rb"`
  mode and read from the saved position), then converted to `double`.
  The previously loaded image is freed first and the new image is fully selected.
* **SELECT (select.c):** the coordinates are read as strings and checked to
  contain only digits. `validate_coordinates()` swaps them if they are given in
  reverse order, rejects empty areas and areas outside the image, and makes the
  end coordinates inclusive. If the selection is invalid, the previous one is kept.
  `SELECT ALL` selects the whole image.
* **HISTOGRAM (histogram.c):** `read_histogram_values()` reads and validates the
  two numbers (`x` = maximum number of stars, `y` = number of bins, which must be
  a power of 2 between 2 and 256; otherwise `Invalid set of parameters` is
  printed). The frequency of each bin
  is computed with an interval of `(max + 1) / bins`, and the number of stars on
  a line is `frequency / max_frequency * max_stars`, truncated. Color images are
  rejected.
* **EQUALIZE (equalize.c):** builds the histogram of the whole image and replaces
  every value `a` with `255 * (H(0) + ... + H(a)) / area`, clamped by `clamp()`.
  The selection is ignored and not changed.
* **ROTATE (rotate.c):** the angle is brought into `[0, 360)`, so each 90
  degrees is a clockwise quarter turn (`-90` becomes `270`). If the whole image
  is selected, `rotate_90()` builds a new matrix with swapped dimensions; if
  the selection is square, `rotate_selection()` rotates only that area through a
  temporary copy. Any other selection prints `The selection must be square`, and
  angles other than the multiples of 90 in `[-360, 360]` print
  `Unsupported rotation angle`.
* **CROP (crop.c):** copies the selected area into new matrices with
  `copy_section()`, frees the old image and updates the dimensions and the
  selection (which becomes the whole new image).
* **APPLY (apply.c):** the four 3x3 kernels are stored in a table. For every
  pixel of the selection that has all its 8 neighbours, `apply_pixel()` multiplies
  the neighbourhood with the kernel, sums the result and clamps it to
  `[0, 255]`, for each of the 3 channels. The new values are written in a copy
  of the image and then copied back, so already modified pixels are never used
  as neighbours. Pixels on the border of the image stay unchanged. Grayscale
  images print `Easy, Charlie Chaplin` and a missing or unknown effect name prints
  `APPLY parameter invalid`.
* **SAVE (save.c):** writes the header (magic word chosen from the image type and
  the requested format, dimensions and maximum value) and then the pixels, as
  text if `ascii` was given and as bytes otherwise, regardless of the format of
  the loaded file.
* **EXIT (exit.c):** frees all the matrices of the image.
* **Memory management:** matrices are allocated with `malloc` / `calloc` and
  freed as soon as they are not needed anymore. Every function that replaces an
  image (`LOAD`, `CROP`, `ROTATE`) frees the old one, and the temporary copies
  made by `APPLY` and `ROTATE` are freed before the command ends.

## Conclusion

The editor is split in one module per command, with the image kept in
dynamically allocated matrices and no global variables.