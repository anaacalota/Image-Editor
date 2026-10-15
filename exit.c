#include "exit.h"
#include "crop.h"

/*
 * Frees the matrices of the image (only `r` unless the image is color).
 */
void free_memory(pixel_dbl pixels_matrix, int height, int image_type)
{
	free_double(pixels_matrix.r, height);

	if (image_type == 2) {
		free_double(pixels_matrix.g, height);
		free_double(pixels_matrix.b, height);
	}
}

/*
 * EXIT: frees the image if one is loaded, otherwise prints an error.
 */
void exit_img(pixel_dbl pixels_matrix, int height, int image_type, int ok)
{
	if (ok == 0) {
		printf("No image loaded\n");
	} else {
		free_memory(pixels_matrix, height, image_type);
	}
}
