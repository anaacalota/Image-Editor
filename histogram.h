#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "struct.h"

/**
 * Reads the number of bins and the maximum number of stars.
 */
int read_histogram_values(int *nr_bins, int *max_stars);

/**
 * Prints the histogram of a black and white / grayscale image.
 */
void print_histogram(pixel_dbl pixels_matrix, int height, int width,
					 int max_value, int image_type);

#endif
