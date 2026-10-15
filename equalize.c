#include "equalize.h"

/*
 * Limits x to the interval [0, 255].
 */
double clamp(double x)
{
	if (x < 0)
		return 0;
	if (x > 255)
		return 255;
	return x;
}

/*
 * Equalizes the whole image (the selection is ignored and not changed).
 * The new value of a pixel with value a is
 * f(a) = 255 * (H(0) + ... + H(a)) / area
 * where H(i) is the number of pixels with value i.
 */
void equalize(pixel_dbl *pixels_matrix, int height, int width, int image_type)
{
	// only black and white / grayscale images can be equalized
	if (image_type == 2) {
		printf("Black and white image needed\n");
		return;
	}

	// histogram: H[v] = number of pixels with value v
	int max_val = 255;
	int *H = (int *)calloc(max_val + 1, sizeof(int));
	if (!H) {
		fprintf(stderr, "malloc failed\n");
		return;
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int value = (int)round(pixels_matrix->r[i][j]);
			H[value]++;
		}
	}

	// new value of every gray level: T[a] = 255 * (H[0] + ... + H[a]) / area
	int area = height * width;
	double *T = (double *)calloc(max_val + 1, sizeof(double));
	if (!T) {
		fprintf(stderr, "malloc failed\n");
		free(H);
		return;
	}

	int sum = 0;
	for (int i = 0; i <= max_val; i++) {
		sum += H[i];
		T[i] = 255.0 * sum / area;
	}

	// replace every pixel with its new value (clamped to [0, 255])
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int value = (int)round(pixels_matrix->r[i][j]);
			pixels_matrix->r[i][j] = clamp(T[value]);
		}
	}

	free(H);
	free(T);

	printf("Equalize done\n");
}
