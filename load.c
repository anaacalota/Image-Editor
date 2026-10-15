#include "load.h"

/*
 * Frees an unsigned char matrix with `height` rows.
 */
void free_matrix(unsigned char **pixels, int height)
{
	if (pixels) {
		for (int i = 0; i < height; i++) {
			free(pixels[i]);
		}
		free(pixels);
	}
}

/*
 * Skips the comment lines (starting with '#') at the current position of
 * the file. If there is no comment, the character that was read to check
 * for one is put back.
 */
void comments(FILE *file)
{
	int ok = 1, nr_comments = 0;
	char character;

	while (ok == 1) { // keep reading while there are consecutive comment lines
		ok = 0;
		fscanf(file, "%c", &character);

		if (character == '#') {
			ok = 1;
			nr_comments++;

			while (character != '\n') {
				fscanf(file, "%c", &character);
			}
		}
	}

	if (nr_comments == 0) {
		fseek(file, -1, SEEK_CUR);
	}
}

/*
 * Returns 0 for ASCII files (P1, P2, P3) and 1 for binary ones (P4, P5, P6).
 */
int find_file_type(char type[LEN])
{
	if (strcmp("P1", type) == 0 ||
		strcmp("P2", type) == 0 ||
		strcmp("P3", type) == 0) {
		return 0; // ASCII
	}
	return 1; // P4, P5 or P6 (binary)
}

/*
 * Returns the image type from the magic word:
 * 0 black and white, 1 grayscale (P2/P5), 2 color (P3/P6).
 */
int find_image_type(const char type[LEN])
{
	if (type[0] == 'P' && (type[1] == '2' || type[1] == '5')) {
		return 1; // grayscale
	}

	if (type[0] == 'P' && (type[1] == '3' || type[1] == '6')) {
		return 2; // color
	}

	return 0; // black and white
}

/*
 * Allocates a height x width unsigned char matrix. Returns NULL on failure.
 */
unsigned char **matrix_mem_alloc(int height, int width)
{
	unsigned char **pixels = malloc(height * sizeof(unsigned char *));

	if (!pixels) {
		fprintf(stderr, "malloc failed\n");
	}

	for (int i = 0; i < height; ++i) {
		pixels[i] = (unsigned char *)malloc(width * sizeof(unsigned char));

		if (!pixels[i]) {
			fprintf(stderr, "malloc failed\n");
			for (int j = 0; j < i; j++) {
				free(pixels[j]);
			}
			free(pixels);
			return NULL;
		}
	}

	return pixels;
}

/*
 * Reads (ASCII) the values of a grayscale / black and white image.
 */
void read_colorless(int height, int width, FILE *file, unsigned char **r)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fscanf(file, "%hhd", &r[i][j]);
		}
	}
}

/*
 * Reads (ASCII) the R, G, B values of every pixel of a color image.
 */
void read_color(int height, int width, FILE *file, unsigned char **r,
				unsigned char **g, unsigned char **b)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fscanf(file, "%hhd", &r[i][j]);
			fscanf(file, "%hhd", &g[i][j]);
			fscanf(file, "%hhd", &b[i][j]);
		}
	}
}

/*
 * Allocates the matrices needed by the image type and
 * reads the pixels from an ASCII file.
 */
pixel read_ascii_matrix(int width, int height, int image_type,
						FILE *file)
{
	pixel image;

	if (image_type == 0 || image_type == 1) { // black and white or grayscale
		image.r = matrix_mem_alloc(height, width);

		if (!image.r) {
			return image;
		}

		read_colorless(height, width, file, image.r);
	} else { // color
		image.r = matrix_mem_alloc(height, width);
		image.g = matrix_mem_alloc(height, width);
		image.b = matrix_mem_alloc(height, width);

		if (!image.r || !image.g || !image.b) {
			if (image.r) {
				free(image.r);
			}
			if (image.g) {
				free(image.g);
			}
			if (image.b) {
				free(image.b);
			}
			return image;
		}

		read_color(height, width, file, image.r, image.g, image.b);
	}

	return image;
}

/*
 * Reads (binary) the values of a grayscale / black and white image,
 * one byte each.
 */
void read_colorless_binary(int height, int width, FILE *file,
						   unsigned char **r)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fread(&r[i][j], sizeof(unsigned char), 1, file);
		}
	}
}

/*
 * Reads (binary) the R, G, B bytes of every pixel of a color image.
 */
void read_color_binary(int height, int width, FILE *file,
					   unsigned char **r, unsigned char **g,
					   unsigned char **b)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fread(&r[i][j], sizeof(unsigned char), 1, file);
			fread(&g[i][j], sizeof(unsigned char), 1, file);
			fread(&b[i][j], sizeof(unsigned char), 1, file);
		}
	}
}

/*
 * Allocates the matrices needed by the image type and
 * reads the pixels from a binary file.
 */
pixel read_binary_matrix(int width, int height, int image_type,
						 FILE *file)
{
	pixel image;

	if (image_type == 0 || image_type == 1) {
		image.r = matrix_mem_alloc(height, width);
		if (!image.r) {
			return image;
		}

		read_colorless_binary(height, width, file, image.r);
	} else {
		image.r = matrix_mem_alloc(height, width);
		image.g = matrix_mem_alloc(height, width);
		image.b = matrix_mem_alloc(height, width);

		if (!image.r || !image.g || !image.b) {
			if (image.r) {
				free(image.r);
			}
			if (image.g) {
				free(image.g);
			}
			if (image.b) {
				free(image.b);
			}
			return image;
		}

		read_color_binary(height, width, file, image.r,
						  image.g, image.b);
	}

	return image;
}

/*
 * Reads the header after the magic word: width and height, then the
 * maximum value (not present for black and white images, P1 / P4).
 * Comments between the fields are skipped.
 */
void read_dimensions(FILE *file, const char *type, int *width,
					 int *height, int *max)
{
	const char * const max_value_types[] = {"P2", "P3", "P5", "P6"};
	const char * const black_and_white_types[] = {"P1", "P4"};
	int is_max_value_type = 0, is_black_and_white_type = 0;

	for (int i = 0; i < 4; i++) {
		if (strcmp(type, max_value_types[i]) == 0) {
			is_max_value_type = 1;
		}
		if (strcmp(type, black_and_white_types[i]) == 0) {
			is_black_and_white_type = 1;
		}
	}

	if (is_max_value_type) {
		fscanf(file, "%d%d", width, height);
		comments(file);
		fscanf(file, "%d", max);
	} else if (is_black_and_white_type) {
		fscanf(file, "%d%d", width, height);
	}
}

/*
 * Opens the file and reads the header and the pixels. Prints "Loaded" or
 * "Failed to load". On success the whole image becomes the selection
 * ((x1, y1) = (0, 0), (x2, y2) = last pixel). *ok is 1 only if an image was
 * loaded. Binary files are reopened in "rb" mode and read from the saved
 * position.
 */
pixel load(char *filename, int *image_type, int *height, int *width,
		   int *max, int *x1, int *x2, int *y1, int *y2,
		   int *ok_load, int *ok)
{
	FILE *file;
	long position;
	pixel pixels_matrix = {NULL, NULL, NULL};
	char type[5];

	file = fopen(filename, "r");
	if (!file) {
		printf("Failed to load %s\n", filename);
		*ok = 0;
		*ok_load = 0;
		return pixels_matrix;
	}

	comments(file);
	fscanf(file, "%s", type);
	comments(file);

	read_dimensions(file, type, width, height, max);

	// remember the position where the pixel data starts (used for binary files)
	position = ftell(file);

	*image_type = find_image_type(type);
	int file_type = find_file_type(type);

	if (file_type == 0) { // ASCII
		fprintf(stdout, "Loaded %s\n", filename);
		*ok = 1;
		pixels_matrix = read_ascii_matrix(*width, *height,
										  *image_type, file);
	} else { // binary
		fclose(file);
		file = fopen(filename, "rb");
		if (!file) {
			printf("Failed to load %s\n", filename);
			*ok_load = 0;
			*ok = 0;
			return pixels_matrix;
		}
		// reopened in binary mode: continue right after the whitespace that
		// follows the header
		fseek(file, position + 1, SEEK_SET);
		pixels_matrix = read_binary_matrix(*width, *height,
										   *image_type, file);
		*ok = 1;
		fprintf(stdout, "Loaded %s\n", filename);
	}

	fclose(file);

	*x1 = 0;
	*y1 = 0;
	*x2 = *width - 1;
	*y2 = *height - 1;

	return pixels_matrix;
}
