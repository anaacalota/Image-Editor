#ifndef _SELECT_H_
#define _SELECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LENGTH 10

typedef struct {
	int start_x;
	int start_y;
	int end_x;
	int end_y;
} coordinate;

/**
 * Reads one coordinate (as a string) from the input.
 */
void read_position(char position[LENGTH], int *ok1, int *okendline);

/**
 * Discards the rest of the current input line.
 */
void extra_chars(void);

/**
 * Checks that a string contains only digits.
 */
int digits_verify(const char *string);

/**
 * Swaps two ints.
 */
void swap_ints(int *a, int *b);

/**
 * Validates the selection coordinates (ordering them if needed).
 */
int validate_coordinates(int *start_x, int *start_y, int *end_x,
						 int *end_y, int height, int width);

/**
 * Handles the SELECT command.
 */
void select(int *start_x, int *end_x, int *start_y, int *end_y,
			int height, int width);

#endif
