#ifndef _CROP_H_
#define _CROP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct.h"

/*
 * Frees a double matrix.
 */
void free_double(double **matrix, int height);

/*
 * Allocates memory for a double matrix.
 */
double **double_alloc(int height, int width);

/*
 * Copies a section of a double matrix.
 */
void copy_section(int start_line, int start_column, int height,
				  int width, double **source, double **destination);

/*
 * Frees all the channels of a pixel_dbl image.
 */
void free_img(pixel_dbl image, int height, int image_type);

/*
 * Handles the CROP command.
 */
pixel_dbl crop(pixel_dbl image, int *height, int *width, int *x1,
			   int *x2, int *y1, int *y2, int image_type);

#endif
