#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "png.h"

typedef struct 
{
	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;
} Image;

Image alloc_image(int width, int height);
void free_image(Image image);
Image read_png_file(const char *filename);
void write_png_file(const char *filename, Image image);

// printf("PNG_COLOR_TYPE_GRAY: %d\n", PNG_COLOR_TYPE_GRAY);		// 0
// printf("PNG_COLOR_TYPE_PALETTE: %d\n", PNG_COLOR_TYPE_PALETTE);	// 3
// printf("PNG_COLOR_TYPE_RGB: %d\n", PNG_COLOR_TYPE_RGB);			// 2
// printf("PNG_COLOR_TYPE_RGBA: %d\n", PNG_COLOR_TYPE_RGBA);		// 6
// printf("PNG_COLOR_TYPE_GA: %d\n", PNG_COLOR_TYPE_GA);			// 4

#endif
