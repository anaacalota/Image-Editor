#ifndef _SAVE_H_
#define _SAVE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "struct.h"

#define SIZE 50

/**
 * Saves an image in binary format.
 */
void save_binary(int height, int width, int image_type,
				 pixel_dbl pixels_matrix, FILE *out);

/**
 * Saves an image in ASCII format.
 */
void save_ascii(int height, int width, int image_type,
				pixel_dbl pixels_matrix, FILE *out);

/**
 * Handles the SAVE command.
 */
void save(int file_type_save, char filename[SIZE],
		  pixel_dbl pixels_matrix, int height, int width,
		  int image_type, int max);

#endif
