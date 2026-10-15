#ifndef _ROTATE_H_
#define _ROTATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"

/*
 * Allocates the matrices of an image.
 */
pixel_dbl matrix_alloc(int height, int width, int image_type);

/*
 * Frees the matrices of an image.
 */
void free_m(pixel_dbl pixels_matrix, int height, int image_type);

/*
 * Rotates a whole image by 90 degrees clockwise.
 */
pixel_dbl rotate_90(pixel_dbl pixels_matrix, int height, int width,
					int image_type);

/*
 * Rotates a selection by 90 degrees clockwise
 */
pixel_dbl rotate_selection(pixel_dbl pixels_matrix, int x1, int y1, int x2,
						   int y2, int image_type);

/*
 * Handles the ROTATE command.
 */
pixel_dbl rotate(int ok, int image_type, int *x1, int *y1, int *x2,
				 int *y2, int *height, int *width, pixel_dbl pixels_matrix);

#endif
