#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct.h"
#include "load.h"
#include "select.h"
#include "histogram.h"
#include "equalize.h"
#include "rotate.h"
#include "crop.h"
#include "save.h"
#include "exit.h"
#include "apply.h"

/*
 * Reads the first character after the file name of a SAVE command.
 * Returns 1 if the "ascii" parameter is present (the word is consumed),
 * 0 otherwise (binary output).
 */
int find_file_name(void)
{
	char next_ch;
	scanf("%c", &next_ch);

	if (next_ch == 'a') {
		char aux[10];
		scanf("%s", aux);
		return 1; // ASCII
	}

	return 0; // binary
}

/*
 * Reads the file name of a SAVE command and sets *file_type:
 * 1 if the output must be ASCII, 0 if it must be binary.
 */
void read_file_name(int *file_type, char file_to_save[100])
{
	char character;
	scanf("%s", file_to_save);
	scanf("%c", &character);

	if (character == ' ') {
		*file_type = find_file_name();
	} else {
		*file_type = 0;
	}
}

/*
 * LOAD: frees the image already in memory (if any), loads the new one as
 * unsigned char matrices, converts it to double matrices and frees the
 * temporary ones. *ok tells whether an image is loaded afterwards.
 */
void handle_load(pixel_dbl *pixels_matrix, int *ok, int *image_type,
				 int *height, int *width, int *max, int *x1, int *x2,
				 int *y1, int *y2)
{
	pixel tmp_matrix;
	int ok_load = 1;
	char filename[100];
	scanf("%s", filename);

	// free the previously loaded image, if any
	if (*ok == 1) {
		if (*image_type == 0 || *image_type == 1) {
			free_double(pixels_matrix->r, *height);
		} else {
			free_double(pixels_matrix->r, *height);
			free_double(pixels_matrix->g, *height);
			free_double(pixels_matrix->b, *height);
		}
	}

	// load the new image
	tmp_matrix = load(filename, image_type, height, width, max,
					  x1, x2, y1, y2, &ok_load, ok);

	if (ok_load == 1) {
		if (*image_type == 0 || *image_type == 1) {
			pixels_matrix->r = double_alloc(*height, *width);
		} else {
			pixels_matrix->r = double_alloc(*height, *width);
			pixels_matrix->g = double_alloc(*height, *width);
			pixels_matrix->b = double_alloc(*height, *width);
		}

		*pixels_matrix = conversion(*height, *width, *image_type,
									tmp_matrix, *pixels_matrix);

		free_matrix(tmp_matrix.r, *height);
		if (*image_type != 0 && *image_type != 1) {
			free_matrix(tmp_matrix.g, *height);
			free_matrix(tmp_matrix.b, *height);
		}
	}
}

/*
 * SELECT: prints an error if no image is loaded (discarding the rest of
 * the line), otherwise reads and applies the new selection.
 */
void handle_select(int ok, int *x1, int *x2, int *y1, int *y2,
				   int height, int width)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	select(x1, x2, y1, y2, height, width);
}

/*
 * HISTOGRAM: prints an error if no image is loaded,
 * otherwise prints the histogram.
 */
void handle_histogram(pixel_dbl pixels_matrix, int ok, int height,
					  int width, int max, int image_type)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	print_histogram(pixels_matrix, height, width, max, image_type);
}

/*
 * EQUALIZE: prints an error if no image is loaded,
 * otherwise equalizes the image.
 */
void handle_equalize(pixel_dbl *pixels_matrix, int ok, int height,
					 int width, int image_type)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	equalize(pixels_matrix, height, width, image_type);
}

/*
 * ROTATE: rotates the selection (all checks are done inside rotate()).
 */
void handle_rotate(pixel_dbl *pixels_matrix, int ok, int image_type, int *x1,
				   int *y1, int *x2, int *y2, int *height, int *width)
{
	*pixels_matrix = rotate(ok, image_type, x1, y1, x2, y2, height, width,
							*pixels_matrix);
}

/*
 * CROP: prints an error if no image is loaded,
 * otherwise keeps only the selection.
 */
void handle_crop(pixel_dbl *pixels_matrix, int ok, int *height, int *width,
				 int *x1, int *x2, int *y1, int *y2, int image_type)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	*pixels_matrix = crop(*pixels_matrix, height, width, x1, x2, y1,
						  y2, image_type);
}

/*
 * APPLY: prints an error if no image is loaded, otherwise reads the effect
 * name and applies it on the selection if the name is valid.
 */
void handle_apply(pixel_dbl *pixels_matrix, int ok, int height, int width,
				  int x1, int x2, int y1, int y2, int image_type)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	int effect_type = effect_parameter();
	if (effect_type >= 6 && effect_type <= 9) {
		*pixels_matrix = apply(height, width, x1, x2, y1, y2, effect_type,
							   *pixels_matrix, image_type);
	}
}

/*
 * SAVE: prints an error if no image is loaded,
 * otherwise saves the image in the given file.
 */
void handle_save(pixel_dbl pixels_matrix, int ok, int height, int width,
				 int image_type, int max)
{
	if (ok == 0) {
		printf("No image loaded\n");
		extra_chars();
		return;
	}
	int file_type_to_save;
	char file_to_save[100];
	read_file_name(&file_type_to_save, file_to_save);
	save(file_type_to_save, file_to_save, pixels_matrix, height,
		 width, image_type, max);
	printf("Saved %s\n", file_to_save);
}

/*
 * EXIT: frees the image and tells main() to stop reading commands.
 */
void handle_exit(pixel_dbl pixels_matrix, int height, int image_type,
				 int ok, int *ok_exit)
{
	exit_img(pixels_matrix, height, image_type, ok);
	*ok_exit = 1;
}

/*
 * Command loop. Image state kept between commands:
 *   pixels_matrix - the image (one double matrix per channel)
 *   ok            - 1 if an image is loaded, 0 otherwise
 *   image_type    - 0 black and white, 1 grayscale, 2 color
 *   height, width, max - image dimensions and maximum pixel value
 *   (x1, y1)-(x2, y2) - current selection (both corners inclusive)
 */
int main(void)
{
	pixel_dbl pixels_matrix;
	int ok = 0, ok_exit = 0, image_type;
	int x1, x2, y1, y2, height, width, max;
	do {
		char command[20];
		scanf("%s", command);

		if (strcmp(command, "LOAD") == 0) {
			handle_load(&pixels_matrix, &ok, &image_type, &height, &width,
						&max, &x1, &x2, &y1, &y2);
			continue;
		}

		if (strcmp(command, "SELECT") == 0) {
			handle_select(ok, &x1, &x2, &y1, &y2, height, width);
			continue;
		}

		if (strcmp(command, "HISTOGRAM") == 0) {
			handle_histogram(pixels_matrix, ok, height, width,
							 max, image_type);
			continue;
		}

		if (strcmp(command, "EQUALIZE") == 0) {
			handle_equalize(&pixels_matrix, ok, height, width, image_type);
			continue;
		}

		if (strcmp(command, "ROTATE") == 0) {
			handle_rotate(&pixels_matrix, ok, image_type, &x1, &y1, &x2,
						  &y2, &height, &width);
			continue;
		}

		if (strcmp(command, "CROP") == 0) {
			handle_crop(&pixels_matrix, ok, &height, &width, &x1, &x2,
						&y1, &y2, image_type);
			continue;
		}

		if (strcmp(command, "APPLY") == 0) {
			handle_apply(&pixels_matrix, ok, height, width, x1, x2, y1,
						 y2, image_type);
			continue;
		}

		if (strcmp(command, "SAVE") == 0) {
			handle_save(pixels_matrix, ok, height, width, image_type, max);
			continue;
		}

		if (strcmp(command, "EXIT") == 0) {
			handle_exit(pixels_matrix, height, image_type, ok, &ok_exit);
			continue;
		}

		// unknown command
		printf("Invalid command\n");
		extra_chars();
	} while (ok_exit == 0);

	return 0;
}
