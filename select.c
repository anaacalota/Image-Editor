#include "select.h"

/*
 * Reads one coordinate (as a string) from the input. *ok1 becomes 0 if there
 * is no coordinate left; *okendline becomes 0 if the end of the line was
 * reached.
 */
void read_position(char position[LENGTH], int *ok1, int *okendline)
{
	char character;

	// reads the first character
	character = getchar();

	if (character == ' ') {
		// reads up to the next space
		if (scanf("%s", position) != 1) {
			*ok1 = 0;
		}
	} else {
		*ok1 = 0;
		if (character == '\n') {
			*okendline = 0;
		}
	}
}

/*
 * Discards the characters left on the current input line.
 */
void extra_chars(void)
{
	int character;
	while ((character = getchar()) != '\n' && character != EOF) {

	}
}

/*
 * Returns 1 if the string contains only digits, 0 otherwise.
 */
int digits_verify(const char *string)
{
	while (*string) {
		if (isdigit(*string++) == 0)
			return 0;
	}
	return 1;
}

/*
 * Swaps two ints.
 */
void swap_ints(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
 * Checks the coordinates given to SELECT. They must delimit a non-empty
 * area and be inside the image; they may be given in any order, so the
 * start / end values are swapped when needed. The end coordinates are
 * made inclusive (the user gives an interval open on the right).
 * Prints an error and returns 0 if the selection is invalid.
 */
int validate_coordinates(int *start_x, int *start_y, int *end_x,
						 int *end_y, int height, int width)
{
	// the coordinates do not delimit an area
	if (*end_x - *start_x == 0 || *end_y - *start_y == 0) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	if (*start_x > *end_x) {
		swap_ints(start_x, end_x);
	}
	if (*start_y > *end_y) {
		swap_ints(start_y, end_y);
	}

	// the interval is open on the right: make the end coordinates inclusive
	*end_x = *end_x - 1;
	*end_y = *end_y - 1;

	// check that all the coordinates are inside the image
	if (*start_x < 0 || *start_x >= width || *start_y < 0 ||
		*start_y >= height || *end_x < 0 || *end_x >= width ||
		*end_y < 0 || *end_y >= height) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	return 1;
}

/*
 * SELECT <x1> <y1> <x2> <y2> or SELECT ALL. The selection is changed only
 * if the input is valid, otherwise the previous one is kept.
 */
void select(int *start_x, int *end_x, int *start_y, int *end_y,
			int height, int width)
{
	int okendline = 1;
	char position[4][20];
	int ok1 = 1;

	scanf("%s", position[0]);

	// SELECT ALL: the whole image is selected
	if (strcmp(position[0], "ALL") == 0) {
		*start_x = 0;
		*end_x = width - 1;
		*start_y = 0;
		*end_y = height - 1;
		printf("Selected ALL\n");
		return;
	}

	// the first coordinate must not be negative
	if (position[0][0] == '-') {
		printf("Invalid set of coordinates\n");
		extra_chars();
		return;
	}

	// read the remaining coordinates
	for (int i = 1; i < 4 && ok1 == 1; i++) {
		read_position(position[i], &ok1, &okendline);
	}

	// all the coordinates were read
	if (ok1 == 1) {
		for (int i = 0; i < 4; i++) {
			if (digits_verify(position[i]) == 0) {
				printf("Invalid command\n");
				return;
			}
		}

		int coords[4];
		for (int i = 0; i < 4; i++) {
			coords[i] = atoi(position[i]);
		}

		int q = coords[0], w = coords[1];
		int e = coords[2], r = coords[3];

		if (!validate_coordinates(&q, &w, &e, &r, height, width)) {
			return;
		}

		int coord_values[4] = {q, w, e, r};
		int *pointers[4] = {start_x, start_y, end_x, end_y};

		for (int i = 0; i < 4; i++) {
			*pointers[i] = coord_values[i];
		}

		printf("Selected %d %d %d %d\n", *start_x, *start_y, *end_x + 1,
			   *end_y + 1);
	} else {
		printf("Invalid command\n");
		if (okendline == 1)
			extra_chars();
	}
}

