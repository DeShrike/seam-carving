#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include "image.h"

#define PIXEL(i, x, y) i.row_pointers[y][x]

void set_pixel(Image image, int x, int y, int r, int g, int b)
{
   image.row_pointers[y][x * 4 + 0] = r;
   image.row_pointers[y][x * 4 + 1] = g;
   image.row_pointers[y][x * 4 + 2] = b;
   image.row_pointers[y][x * 4 + 3] = 255;
}

void get_pixel(Image image, int x, int y, int* r, int* g, int* b)
{
   *r = image.row_pointers[y][x * 4 + 0];
   *g = image.row_pointers[y][x * 4 + 1];
   *b = image.row_pointers[y][x * 4 + 2];
}

char *args_shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    (*argc) -= 1;
    (*argv) += 1;
    return result;
}

float to_luminance(int r, int g, int b)
{
   float rr = r / 255.0;
   float gg = g / 255.0;
   float bb = b / 255.0;
   return 0.2126 * rr + 0.7152 * gg + 0.0722 * bb;
}

float rgb_to_luminance(uint32_t rgb)
{
   float r = ((rgb >> (8 * 0)) & 0x00FF) / 255.0;
   float g = ((rgb >> (8 * 1)) & 0x00FF) / 255.0;
   float b = ((rgb >> (8 * 2)) & 0x00FF) / 255.0;

   return to_luminance(r, g, b);;
}

Image process(Image input)
{
   float *luminance = malloc(sizeof(*luminance) * input.width * input.height);

   int r, g, b;
   for (int x = 0; x < input.width; ++x)
   {
      for (int y = 0; y < input.height; ++y)
      {
         int ix = x + (y * input.width);
         get_pixel(input, x, y, &r, &g, &b);
         luminance[ix] = to_luminance(r, g, b);
      }
   }

   size_t out_width = input.width;
   size_t out_height = input.height;

   printf("Creating %zu x %zu output image\n", out_width, out_height);
   Image out_image = alloc_image(out_width, out_height);

   for (int x = 0; x < input.width; ++x)
   {
      for (int y = 0; y < input.height; ++y)
      {
         int ix = x + (y * input.width);
         int g = luminance[ix] * 255.0;
         set_pixel(out_image, x, y, g, g, g);
      }
   }

   free(luminance);
   return out_image;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    const char *program = args_shift(&argc, &argv);

    if (argc <= 0)
    {
        fprintf(stderr, "Usage: %s <input.png> <output.png>\n", program);
        fprintf(stderr, "ERROR: no input file is provided\n");
        return 1;
    }

    const char *input_file_path = args_shift(&argc, &argv);

    if (argc <= 0)
    {
        fprintf(stderr, "Usage: %s <input.png> <output.png>\n", program);
        fprintf(stderr, "ERROR: no output file is provided\n");
        return 1;
    }

    const char *output_file_path = args_shift(&argc, &argv);

    printf("Reading %s\n", input_file_path);
	 Image i = read_png_file(input_file_path);
    printf("Dimensions: %d x %d\n", i.width, i.height);

    if (i.bit_depth != 8)
    {
        fprintf(stderr, "Only 8 bit images are supported\n");
    	  free_image(i);
		  abort();
	 }

    Image o = process(i);

    printf("Saving %s\n", output_file_path);
    write_png_file(output_file_path, o);
    free_image(o);

	 free_image(i);
	 return 0;
}
