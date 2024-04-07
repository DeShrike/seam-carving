#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include "image.h"

char *args_shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    (*argc) -= 1;
    (*argv) += 1;
    return result;
}

Image process(Image input)
{
   size_t out_width = input.width * 10;
   size_t out_height = input.height * 10;

   printf("Creating %zu x %zu output image\n", out_width, out_height);
   Image out_image = alloc_image(out_width, out_height);

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
