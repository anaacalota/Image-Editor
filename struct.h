#ifndef _STRUCT_H_
#define _STRUCT_H_

// image stored as double matrices (one per color channel); for black and white
// and grayscale images only `r` is used
struct pixel_dbl {
	double **r, **g, **b;
};

typedef struct pixel_dbl pixel_dbl;

// image as read from / written to a file (one byte per value)
struct pixel {
	unsigned char **r, **g, **b;
};

typedef struct pixel pixel;

#endif
