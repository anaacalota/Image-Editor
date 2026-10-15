#include "crop.h"

/*
 * Frees a double matrix with `height` rows.
 */
void free_double(double **matrix, int height)
{
	if (matrix) {
		for (int i = 0; i < height; i++) {
			if (matrix[i]) {
				free(matrix[i]);
			}
		}
		free(matrix);
	}
}

/*
 * Allocates a height x width double matrix. Returns NULL on failure.
 */
double **double_alloc(int height, int width)
{
	double **matrix = (double **)malloc(height * sizeof(double *));
	if (!matrix) {
		fprintf(stderr, "malloc failed\n");
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		matrix[i] = (double *)malloc(width * sizeof(double));
		if (!matrix[i]) {
			fprintf(stderr, "malloc failed\n");
			for (int j = 0; j < i; j++)
				free(matrix[j]);
			free(matrix);
			return NULL;
		}
	}
	return matrix;
}

/*
 * Copies a height x width area of `source`, whose top-left corner is at
 * (start_line, start_column), into `destination`.
 */
void copy_section(int start_line, int start_column, int height,
				  int width, double **source, double **destination)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			destination[i][j] = source[start_line + i][start_column + j];
}

/*
 * Frees the matrices of an image (g and b only exist for color images).
 */
void free_img(pixel_dbl image, int height, int image_type)
{
	free_double(image.r, height);
	if (image_type == 2) {
		free_double(image.g, height);
		free_double(image.b, height);
	}
}

/*
 * CROP: replaces the image with the current selection. The old image is freed
 * and, on success, "Image cropped" is printed. If an allocation fails the
 * image is left unchanged.
 */
pixel_dbl crop(pixel_dbl image, int *height, int *width, int *x1, int *x2,
			   int *y1, int *y2, int image_type)
{
	int new_height = *y2 - *y1 + 1;
	int new_width = *x2 - *x1 + 1;

	double **r_copy = double_alloc(new_height, new_width);
	if (!r_copy)
		return image;

	double **g_copy = NULL, **b_copy = NULL;
	if (image_type == 2) {
		g_copy = double_alloc(new_height, new_width);
		b_copy = double_alloc(new_height, new_width);
		if (!g_copy || !b_copy) {
			if (g_copy) {
				free_double(g_copy, new_height);
			}
			if (b_copy) {
				free_double(b_copy, new_height);
			}
			free_double(r_copy, new_height);
			return image;
		}
	}

	// copy the selected pixels into the new matrices
	copy_section(*y1, *x1, new_height, new_width, image.r, r_copy);
	if (image_type == 2) {
		copy_section(*y1, *x1, new_height, new_width, image.g, g_copy);
		copy_section(*y1, *x1, new_height, new_width, image.b, b_copy);
	}

	// free the memory of the old image
	free_img(image, *height, image_type);

	// the cropped image replaces the old one
	image.r = r_copy;
	if (image_type == 2) {
		image.g = g_copy;
		image.b = b_copy;
	}

	// update the dimensions; the selection is now the whole image
	*height = new_height;
	*width = new_width;
	*x1 = 0;
	*y1 = 0;
	*x2 = new_width - 1;
	*y2 = new_height - 1;

	printf("Image cropped\n");
	return image;
}
