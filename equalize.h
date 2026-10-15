#ifndef _EQUALIZE_H_
#define _EQUALIZE_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "struct.h"

/*
 * Limits a value to the interval [0, 255]
 */
double clamp(double x);

/**
 * Equalizes the histogram of a grayscale image.
 */
void equalize(pixel_dbl *pixels_matrix, int height, int width,
			  int image_type);

#endif
