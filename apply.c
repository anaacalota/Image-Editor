#include "apply.h"
#include "crop.h"
#include "load.h"

/*
 * Converts the unsigned char matrices read from a file into double matrices.
 */
pixel_dbl conversion(int height, int width, int image_type,
					 pixel pixels_matrix, pixel_dbl tmp_matrix)
{
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			tmp_matrix.r[i][j] = (double)pixels_matrix.r[i][j];
		}
	}

	if (image_type == 2) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				tmp_matrix.g[i][j] = (double)pixels_matrix.g[i][j];
				tmp_matrix.b[i][j] = (double)pixels_matrix.b[i][j];
			}
		}
	}

	return tmp_matrix;
}

/*
 * Multiplies a 3x3 pixel neighbourhood element by element with a kernel,
 * sums the products and clamps the result to [0, 255].
 */
double apply_pixel(double kernel[VAL][VAL],
				   double pixel_matrix[VAL][VAL])
{
	double result = 0.0;

	for (int i = 0; i < VAL; i++) {
		for (int j = 0; j < VAL; j++) {
			double pixel_contribution = kernel[i][j] * pixel_matrix[i][j];
			result += pixel_contribution;
		}
	}

	result = clamp(result);

	return result;
}

/*
 * Copies the 3x3 neighbourhood of pixel (i, j) from one channel into
 * tmp_matrix (values outside the image are taken as 0).
 */
void kernel_preparation(double **pixels_matrix, int i, int j,
						double tmp_matrix[VAL][VAL], int height,
						int width)
{
	for (int k = -1; k <= 1; k++) {
		for (int l = -1; l <= 1; l++) {
			int x = i + k;
			int y = j + l;
			if (x >= 0 && x < height && y >= 0 && y < width) {
				tmp_matrix[k + 1][l + 1] = pixels_matrix[x][y];
			} else {
				tmp_matrix[k + 1][l + 1] = 0;
			}
		}
	}
}

/*
 * Reads the parameter of APPLY. Returns 6 (EDGE), 7 (SHARPEN), 8 (BLUR) or
 * 9 (GAUSSIAN_BLUR); prints an error and returns 55 if the parameter is
 * missing or not valid ("APPLY parameter invalid").
 */
int effect_parameter(void)
{
	// supported effects (the effect type is 6 + the index in this array)
	static const char * const effects[] = {"EDGE", "SHARPEN", "BLUR",
										"GAUSSIAN_BLUR"};

	char parameter[20];
	char c;

	scanf("%c", &c);

	if (c == ' ') {
		scanf("%s", parameter);

		for (int i = 0; i < NR; i++) {
			if (strcmp(parameter, effects[i]) == 0) {
				return 6 + i;
			}
		}

		// invalid effect name
		printf("APPLY parameter invalid\n");
		return 55; // error
	}

	// no parameter was given
	printf("Invalid command\n");
	return 55; // error
}

/*
 * Loads into the given 3x3 matrix the kernel of the effect (type 6 - 9).
 */
void effect_initialization(double pixels_matrix[VAL][VAL], int effect_type)
{
	double edge[VAL][VAL] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	double sharpen[VAL][VAL] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
	double blur[VAL][VAL] = {{1.0 / 9, 1.0 / 9, 1.0 / 9},
	{1.0 / 9, 1.0 / 9, 1.0 / 9}, {1.0 / 9, 1.0 / 9, 1.0 / 9}};
	double gaussian_blur[VAL][VAL] = {{1.0 / 16, 1.0 / 8, 1.0 / 16},
	{1.0 / 8, 1.0 / 4, 1.0 / 8}, {1.0 / 16, 1.0 / 8, 1.0 / 16}};

	int index = effect_type - 6;

	if (index == 0) {
		cpy(pixels_matrix, edge);
	}
	if (index == 1) {
		cpy(pixels_matrix, sharpen);
	}
	if (index == 2) {
		cpy(pixels_matrix, blur);
	}
	if (index == 3) {
		cpy(pixels_matrix, gaussian_blur);
	}
}

/*
 * Copies a 3x3 matrix.
 */
void cpy(double pixels_matrix[VAL][VAL], double tmp_matrix[VAL][VAL])
{
	for (int i = 0; i < VAL; i++) {
		memcpy(pixels_matrix[i], tmp_matrix[i], VAL * sizeof(double));
	}

}

/*
 * Computes the new value of pixel (i, j) for the r, g and b channels and
 * stores it in edited_img. Pixels on the border of the image are skipped.
 */
void edited_image(int height, int width, int i, int j,
				  double tmp_matrix[VAL][VAL], int effect_type,
				  double effect[NR][VAL][VAL], pixel_dbl edited_img,
				  pixel_dbl pixels_matrix)
{
	int index = effect_type - 6;

	if (i > 0 && i < height - 1 && j > 0 && j < width - 1) {
		for (int c = 0; c < VAL; c++) {
			// pick the r, g or b channel
			double **channel;
			switch (c) {
			case 0:
				channel = pixels_matrix.r;
				break;
			case 1:
				channel = pixels_matrix.g;
				break;
			case 2:
				channel = pixels_matrix.b;
				break;
			}

			// load the 3x3 neighbourhood of pixel (i, j)
			kernel_preparation(channel, i, j, tmp_matrix, height, width);

			// apply the kernel on this channel and store the result in the
			// edited copy of the image
			switch (c) {
			case 0:
				edited_img.r[i][j] = apply_pixel(tmp_matrix, effect[index]);
				break;
			case 1:
				edited_img.g[i][j] = apply_pixel(tmp_matrix, effect[index]);
				break;
			case 2:
				edited_img.b[i][j] = apply_pixel(tmp_matrix, effect[index]);
				break;
			}
		}
	}
}

/*
 * Applies the chosen effect on the current selection of a color image.
 * The new values are computed from the original ones into a copy and then
 * written back, so a pixel never uses already modified neighbours.
 * Prints "APPLY <effect> done" (or "Easy, Charlie Chaplin" for grayscale
 * images, which are left unchanged).
 */
pixel_dbl apply(int height, int width, int x1, int x2, int y1, int y2,
				int effect_type, pixel_dbl pixels_matrix, int image_type)
{
	pixel_dbl edited_img;

	edited_img.r = double_alloc(height, width);
	edited_img.g = double_alloc(height, width);
	edited_img.b = double_alloc(height, width);

	if (!edited_img.r || !edited_img.g || !edited_img.b) {
		if (edited_img.r) {
			free_double(edited_img.r, height);
		}
		if (edited_img.g) {
			free_double(edited_img.g, height);
		}
		if (edited_img.b) {
			free_double(edited_img.b, height);
		}
		return edited_img;
	}

	double tmp_matrix[VAL][VAL];
	double effect[NR][VAL][VAL]; // the 4 image kernels
	// initialize the kernels for the 4 effects
	for (int i = 0; i < NR; i++) {
		effect_initialization(effect[i], 6 + i);
	}
	if (image_type != 1) {
		for (int i = y1; i <= y2; ++i) {
			for (int j = x1; j <= x2; ++j) {
				edited_image(height, width, i, j, tmp_matrix, effect_type,
							 effect, edited_img, pixels_matrix);
			}
		}

		// copy the new values back into the image (the pixels on the border
		// of the image have no complete neighbourhood and stay unchanged)
		for (int i = y1; i <= y2; ++i) {
			for (int j = x1; j <= x2; ++j) {
				if (i != 0 && j != 0 && i != height - 1 && j != width - 1) {
					pixels_matrix.r[i][j] = edited_img.r[i][j];
					if (pixels_matrix.g && pixels_matrix.b) {
						pixels_matrix.g[i][j] = edited_img.g[i][j];
						pixels_matrix.b[i][j] = edited_img.b[i][j];
					}
				}
			}
		}
		static const char * const effects[] = {"EDGE", "SHARPEN", "BLUR",
									"GAUSSIAN_BLUR"};
		if (effect_type >= 6 && effect_type <= 9) {
			printf("APPLY %s done\n", effects[effect_type - 6]);
		}
	} else {
		printf("Easy, Charlie Chaplin\n");
	}

	// free the edited copy
	free_double(edited_img.r, height);
	if (edited_img.g) {
		free_double(edited_img.g, height);
	}
	if (edited_img.b) {
		free_double(edited_img.b, height);
	}

	return pixels_matrix;
}
