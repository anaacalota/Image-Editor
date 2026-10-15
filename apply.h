#ifndef _APPLY_H_
#define _APPLY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct.h"
#include "load.h"
#include "equalize.h"

#define VAL 3
#define NR 4

/*
 * Converts the unsigned char matrices read from a file into double matrices.
 */
pixel_dbl conversion(int height, int width, int image_type,
					 pixel pixels_matrix, pixel_dbl tmp_matrix);

/*
 * Multiplies a 3x3 pixel neighbourhood with a kernel and sums the result.
 */
double apply_pixel(double pixels_matrix1[VAL][VAL],
				   double pixels_matrix2[VAL][VAL]);

/*
 * Copies the 3x3 neighbourhood of pixel (i, j) into `tmp_matrix`.
 */
void kernel_preparation(double **pixels_matrix, int i, int j,
						double tmp_matrix[VAL][VAL], int height, int width);

/*
 * Reads the APPLY parameter; returns 6-9 for a valid effect, 55 on error.
 */
int effect_parameter(void);

/*
 * Copies a 3x3 matrix.
 */
void cpy(double pixels_matrix[VAL][VAL], double tmp_matrix[VAL][VAL]);

/*
 * Loads the kernel of the given effect.
 */
void effect_initialization(double pixels_matrix[VAL][VAL], int effect_type);

/*
 * Applies the effect on pixel (i, j) for all 3 color channels.
 */
void edited_image(int height, int width, int i, int j,
				  double tmp_matrix[VAL][VAL], int effect_type,
				  double effect[NR][VAL][VAL], pixel_dbl edited_img,
				  pixel_dbl pixels_matrix);

/*
 * Handles the APPLY command.
 */
pixel_dbl apply(int height, int width, int x1, int x2, int y1, int y2,
				int effect_type, pixel_dbl pixels_matrix, int image_type);

#endif
