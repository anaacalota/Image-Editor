#ifndef _EXIT_H_
#define _EXIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"

/**
 * Frees the memory of an image stored as double matrices.
 */
void free_memory(pixel_dbl pixels_matrix, int height, int image_type);

/**
 * Handles the EXIT command.
 */
void exit_img(pixel_dbl pixels_matrix, int height, int image_type, int ok);

#endif
