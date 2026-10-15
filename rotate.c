#include "rotate.h"

/*
 * Allocates the matrices of an image: only `r`,
 * or `r`, `g`, `b` for color images.
 */
pixel_dbl matrix_alloc(int height, int width, int image_type)
{
	pixel_dbl pixels_matrix;
	pixels_matrix.r = (double **)malloc(height * sizeof(double *));
	if (!pixels_matrix.r) {
		fprintf(stderr, "malloc failed\n");
		return pixels_matrix;
	}
	for (int i = 0; i < height; i++) {
		pixels_matrix.r[i] = (double *)malloc(width * sizeof(double));
		if (!pixels_matrix.r[i]) {
			fprintf(stderr, "malloc failed\n");
			for (int j = 0; j < i; j++) {
				free(pixels_matrix.r[j]);
			}
			free(pixels_matrix.r);
			return pixels_matrix;
		}
	}
	if (image_type == 2) {
		pixels_matrix.g = (double **)malloc(height * sizeof(double *));
		pixels_matrix.b = (double **)malloc(height * sizeof(double *));
		if (!pixels_matrix.g || !pixels_matrix.b) {
			fprintf(stderr, "malloc failed\n");
			for (int i = 0; i < height; i++) {
				free(pixels_matrix.r[i]);
			}
			free(pixels_matrix.r);
			if (pixels_matrix.g) {
				free(pixels_matrix.g);
			}
			if (pixels_matrix.b) {
				free(pixels_matrix.b);
			}
			return pixels_matrix;
		}
		for (int i = 0; i < height; i++) {
			pixels_matrix.g[i] = (double *)malloc(width * sizeof(double));
			pixels_matrix.b[i] = (double *)malloc(width * sizeof(double));
			if (!pixels_matrix.g[i] || !pixels_matrix.b[i]) {
				fprintf(stderr, "malloc failed\n");
				for (int j = 0; j < i; j++) {
					free(pixels_matrix.r[j]);
					if (pixels_matrix.g[i]) {
						free(pixels_matrix.g[j]);
					}
					if (pixels_matrix.b[i]) {
						free(pixels_matrix.b[j]);
					}
				}
				free(pixels_matrix.r);
				free(pixels_matrix.g);
				free(pixels_matrix.b);
				return pixels_matrix;
			}
		}
	}
	return pixels_matrix;
}

/*
 * Frees the matrices of an image.
 */
void free_m(pixel_dbl pixels_matrix, int height, int image_type)
{
	for (int i = 0; i < height; i++) {
		free(pixels_matrix.r[i]);
		if (image_type == 2) {
			free(pixels_matrix.g[i]);
			free(pixels_matrix.b[i]);
		}
	}
	free(pixels_matrix.r);
	if (image_type == 2) {
		free(pixels_matrix.g);
		free(pixels_matrix.b);
	}
}

/*
 * Rotates the whole image by 90 degrees clockwise. The result has the
 * dimensions swapped (width x height); the old matrices are freed.
 */
pixel_dbl rotate_90(pixel_dbl pixels_matrix, int height,
					int width, int image_type)
{
	pixel_dbl tmp_matrix = matrix_alloc(width, height, image_type);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			tmp_matrix.r[j][height - 1 - i] = pixels_matrix.r[i][j];
			if (image_type == 2) {
				tmp_matrix.g[j][height - 1 - i] = pixels_matrix.g[i][j];
				tmp_matrix.b[j][height - 1 - i] = pixels_matrix.b[i][j];
			}
		}
	}
	free_m(pixels_matrix, height, image_type);
	return tmp_matrix;
}

/*
 * Rotates the selection (x1, y1)-(x2, y2) by 90 degrees clockwise:
 * copies it into a temporary image, rotates the copy and writes it back
 * over the selected area.
 */
pixel_dbl rotate_selection(pixel_dbl pixels_matrix, int x1, int y1, int x2,
						   int y2, int image_type)
{
	int height = y2 - y1 + 1;
	int width = x2 - x1 + 1;
	pixel_dbl tmp_matrix = matrix_alloc(height, width, image_type);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			tmp_matrix.r[i][j] = pixels_matrix.r[y1 + i][x1 + j];
			if (image_type == 2) {
				tmp_matrix.g[i][j] = pixels_matrix.g[y1 + i][x1 + j];
				tmp_matrix.b[i][j] = pixels_matrix.b[y1 + i][x1 + j];
			}
		}
	}
	pixel_dbl rotated_matrix = rotate_90(tmp_matrix, height, width,
										  image_type);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pixels_matrix.r[y1 + i][x1 + j] = rotated_matrix.r[i][j];
			if (image_type == 2) {
				pixels_matrix.g[y1 + i][x1 + j] = rotated_matrix.g[i][j];
				pixels_matrix.b[y1 + i][x1 + j] = rotated_matrix.b[i][j];
			}
		}
	}
	free_m(rotated_matrix, height, image_type);
	return pixels_matrix;
}

/*
 * ROTATE <angle>. The angle is read first, so the input line is consumed
 * even when no image is loaded. Every 90 degrees is one clockwise quarter
 * turn (negative angles are brought into [0, 360) first).
 *   - whole image selected: the image is rotated and height / width are
 *     swapped;
 *   - square selection: only the selected area is rotated;
 * Prints "Unsupported rotation angle" if the angle is not a multiple of 90.
 */
pixel_dbl rotate(int ok, int image_type, int *x1, int *y1, int *x2,
				 int *y2, int *height, int *width, pixel_dbl pixels_matrix)
{
	int angle;

	fscanf(stdin, "%d", &angle);
	if (ok == 0) {
		printf("No image loaded\n");
		return pixels_matrix;
	}

	int original_angle = angle;

	// bring the angle in [0, 360): -90 becomes 270, which is three clockwise
	// quarter turns (= one counterclockwise quarter turn)
	angle = (angle % 360 + 360) % 360;

	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return pixels_matrix;
	}

	if (*x1 == 0 && *y1 == 0 && *x2 == *width - 1 && *y2 == *height - 1) {
		for (int i = 0; i < angle / 90; i++) {
			pixels_matrix = rotate_90(pixels_matrix, *height,
									  *width, image_type);
			int temp = *height;
			*height = *width;
			*width = temp;
			*x1 = 0;
			*y1 = 0;
			*x2 = *width - 1;
			*y2 = *height - 1;
		}
		printf("Rotated %d\n", original_angle);
	} else if ((*x2 - *x1) == (*y2 - *y1)) {
		for (int i = 0; i < angle / 90; i++) {
			pixels_matrix = rotate_selection(pixels_matrix, *x1, *y1,
											 *x2, *y2, image_type);
		}
		printf("Rotated %d\n", original_angle);
	} else {
		int size;

		if ((*x2 - *x1) < (*y2 - *y1)) {
			size = (*x2 - *x1);
		} else {
			size = (*y2 - *y1);
		}

		*x2 = *x1 + size;
		*y2 = *y1 + size;
		printf("Rotated %d\n", original_angle);
	}

	return pixels_matrix;
}
