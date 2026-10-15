#ifndef _LOAD_H
#define _LOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"

#define LEN 3

/**
 * Frees the memory allocated for a matrix of unsigned char.
 */
void free_matrix(unsigned char **pixels, int height);

/**
 * Skips the comment lines (starting with '#')
 */
void comments(FILE *file);

/**
 * Returns 0 for ASCII files (P1-P3) and 1 for binary ones (P4-P6).
 */
int find_file_type(char type[LEN]);

/**
 * Returns 0 for black and white, 1 for grayscale and 2 for color images.
 */
int find_image_type(const char type[LEN]);

/**
 * Allocates memory for a matrix of unsigned char.
 */
unsigned char **matrix_mem_alloc(int height, int width);

/**
 * Reads the values of a grayscale / black and white image (ASCII).
 */
void read_colorless(int height, int width, FILE *file, unsigned char **r);

/**
 * Reads the values of a color image (ASCII).
 */
void read_color(int height, int width, FILE *file, unsigned char **r,
				unsigned char **g, unsigned char **b);

/**
 * Allocates and reads the pixel matrices of an ASCII image.
 */
pixel read_ascii_matrix(int width, int height, int image_type,
						FILE *file);

/**
 * Reads the values of a grayscale / black and white image (binary).
 */
void read_colorless_binary(int height, int width, FILE *file,
						   unsigned char **r);

/**
 * Reads the values of a color image (binary).
 */
void read_color_binary(int height, int width, FILE *file,
					   unsigned char **r, unsigned char **g,
					   unsigned char **b);

/**
 * Allocates and reads the pixel matrices of a binary image.
 */
pixel read_binary_matrix(int width, int height, int image_type,
						 FILE *file);

/**
 * Reads the header of the image (dimensions and maximum value).
 */
void read_dimensions(FILE *file, const char *type, int *width,
					 int *height, int *max);

/**
 * Loads an image from a file.
 */
pixel load(char *filename, int *image_type, int *height, int *width,
		   int *max, int *x1, int *x2, int *y1, int *y2,
		   int *ok_load, int *ok);

#endif
