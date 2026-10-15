#include "save.h"

/*
 * Writes the pixels as bytes (values are rounded), row by row.
 */
void save_binary(int height, int width, int image_type,
				 pixel_dbl pixels_matrix, FILE *out)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (image_type == 0 || image_type == 1) {
				unsigned char pixel =
				(unsigned char)round(pixels_matrix.r[i][j]);
				fwrite(&pixel, sizeof(char), 1, out);
			} else { // color
				unsigned char pixel_r =
				(unsigned char)round(pixels_matrix.r[i][j]);
				unsigned char pixel_g =
				(unsigned char)round(pixels_matrix.g[i][j]);
				unsigned char pixel_b =
				(unsigned char)round(pixels_matrix.b[i][j]);
				fwrite(&pixel_r, sizeof(char), 1, out);
				fwrite(&pixel_g, sizeof(char), 1, out);
				fwrite(&pixel_b, sizeof(char), 1, out);
			}
		}
	}
}

/*
 * Writes the pixels as text (values are rounded), one image row per line.
 */
void save_ascii(int height, int width, int image_type,
				pixel_dbl pixels_matrix, FILE *out)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (image_type == 0 || image_type == 1) {
				int pixel = (int)round(pixels_matrix.r[i][j]);
				fprintf(out, "%d ", pixel);
			} else { // color
				int pixel_r = (int)round(pixels_matrix.r[i][j]);
				int pixel_g = (int)round(pixels_matrix.g[i][j]);
				int pixel_b = (int)round(pixels_matrix.b[i][j]);
				fprintf(out, "%d %d %d ", pixel_r, pixel_g, pixel_b);
			}
		}
		fprintf(out, "\n");
	}
}

/*
 * Saves the image: header (magic word, dimensions and, except for black
 * and white images, the maximum value) followed by the pixels.
 * file_type_save: 0 = binary, 1 = ASCII.
 */
void save(int file_type_save, char filename[SIZE],
		  pixel_dbl pixels_matrix, int height, int width,
		  int image_type, int max)
{
	FILE *out = fopen(filename, "wt");
	if (!out) {
		fprintf(stderr, "fopen fail %s\n", filename);
		return;
	}

	// write the header: the magic word depends on the image type and on
	// the file type
	fprintf(out, "P");
	if (file_type_save == 0) { // binary
		if (image_type == 0) {
			fprintf(out, "4\n");
		} else if (image_type == 1) {
			fprintf(out, "5\n");
		} else if (image_type == 2) {
			fprintf(out, "6\n");
		}
	} else { // ASCII
		if (image_type == 0) {
			fprintf(out, "1\n");
		} else if (image_type == 1) {
			fprintf(out, "2\n");
		} else if (image_type == 2) {
			fprintf(out, "3\n");
		}
	}

	fprintf(out, "%d %d\n", width, height);
	if (image_type != 0) {
		fprintf(out, "%d\n", max);
	}

	// reopen the file in binary append mode to write the pixel data
	if (file_type_save == 0) {
		fclose(out);
		out = fopen(filename, "ab");
		if (!out) {
			fprintf(stderr, "fopen fail %s\n",
					filename);
			return;
		}
		// save in binary format
		save_binary(height, width, image_type, pixels_matrix, out);
	} else {
		// save in ASCII format
		save_ascii(height, width, image_type, pixels_matrix, out);
	}

	fclose(out);
}
