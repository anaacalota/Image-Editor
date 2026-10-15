#include "histogram.h"
#include "select.h"

/*
 * Reads the rest of the line character by character and checks that it holds
 * exactly two non-negative numbers: HISTOGRAM <x> <y>, where x is the maximum
 * number of stars (stored in *max_stars) and y is the number of bins (stored
 * in *nr_bins). y must be a power of 2 between 2 and 256.
 * Returns 1 if the line is valid, 0 otherwise.
 */
int read_histogram_values(int *max_stars, int *nr_bins)
{
	int character;
	char buffer[100];
	int index = 0;
	int nr_read_values = 0;

	*nr_bins = 0;
	*max_stars = 0;

	while ((character = getchar()) != '\n') {
		// a space ends a number: process the one stored in the buffer
		if (character == ' ') {
			if (index > 0) { // if there is a value in the buffer
				buffer[index] = '\0'; // end the string
				if (!digits_verify(buffer)) {
					return 0;
				}

				if (nr_read_values == 0) {
					*nr_bins = atoi(buffer);
				} else if (nr_read_values == 1) {
					*max_stars = atoi(buffer);
				} else {
					return 0; // invalid if there are more than 2 numbers
				}

				nr_read_values++;
				index = 0;
			}
		} else if (isdigit(character)) {
			if (index < (int)(sizeof(buffer) - 1)) {
				buffer[index++] = character;
			} else {
				return 0; // invalid if the number is too long
			}
		} else {
			return 0;
		}
	}

	// process the last number, if any
	if (index > 0) {
		buffer[index] = '\0';
		if (!digits_verify(buffer)) {
			return 0; // not a number
		}

		if (nr_read_values == 0) {
			*nr_bins = atoi(buffer);
		} else if (nr_read_values == 1) {
			*max_stars = atoi(buffer);
		} else {
			return 0; // more than 2 numbers
		}

		nr_read_values++;
	}

	// exactly 2 values must have been read
	if (nr_read_values != 2) {
		return 0;
	}

	return 1;
}

/**
 * Prints the histogram of a black and white / grayscale image.
 */
void print_histogram(pixel_dbl pixels_matrix, int height, int width,
					 int max_value, int image_type)
{
	int nr_bins, max_stars;

	if (!read_histogram_values(&nr_bins, &max_stars)) {
		printf("Invalid command\n");
		return;
	}

	if (image_type == 2) {
		printf("Black and white image needed\n");
		return;
	}

	// frequency of each bin (initialized to 0)
	int *frequencies = (int *)calloc(nr_bins, sizeof(int));
	if (!frequencies) {
		fprintf(stderr, "malloc failed\n");
		return;
	}

	// every bin covers `interval` consecutive values
	double interval = (double)(max_value + 1) / nr_bins;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// black and white / grayscale images only use the `r` channel
			int bin_index = (int)(pixels_matrix.r[i][j] / interval);
			frequencies[bin_index]++;
		}
	}

	// find the maximum frequency
	int max_frequency = 0;
	for (int i = 0; i < nr_bins; i++) {
		if (frequencies[i] > max_frequency)
			max_frequency = frequencies[i];
	}
	// print the histogram
	for (int i = 0; i < nr_bins; i++) {
		// number of stars, truncated: frequency / max_frequency * max_stars
		int num_stars = (int)floor((double)frequencies[i] /
						max_frequency * max_stars);

		// print the number of stars, the separator, and the stars themselves
		printf("%d\t|\t", num_stars);
		for (int j = 0; j < num_stars; j++) {
			printf("*");
		}
		printf("\n");
	}

	free(frequencies);
}
