#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "image.h"

#define PIXEL(i, x, y) i.row_pointers[y][x]

static char temp_str[100];

static float gx[3][3] = { 
   { 1.0, 0.0, -1.0 },
   { 2.0, 0.0, -2.0 },
   { 1.0, 0.0, -1.0 }
};

static float gy[3][3] = { 
   { 1.0, 2.0, 1.0 },
   { 0.0, 0.0, 0.0 },
   { -1.0, -2.0, -1.0 }
};

bool is_png(const char *filename)
{
   int ix = strrchr(filename, '.') - filename;
   return ix > 0 && strcmp(filename + ix, ".png") == 0;
}

char* add_infix(const char *filename, const char *infix)
{
   strcpy(temp_str, filename);
   int ix = strrchr(filename, '.') - filename;
   strcpy(temp_str + ix, infix);
   strcat(temp_str, ".png");  
   return temp_str;
}

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

Image calc_luminance(Image input)
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

   printf("Creating %u x %u luminance image\n", input.width, input.height);
   Image out_image = alloc_image(input.width, input.height);

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

Image calc_sobel(Image input)
{
   printf("Creating %u x %u sobel image\n", input.width, input.height);
   Image out_image = alloc_image(input.width, input.height);

   int r, g, b;
   for (int cx = 0; cx < input.width; ++cx)
   {
      for (int cy = 0; cy < input.height; ++cy)
      {
         int sobelx = 0, sobely  = 0;
         for (int dx = -1; dx <= 1; ++dx)
         {
            for (int dy = -1; dy <= 1; ++dy)
            {
               int xx = cx + dx;
               int yy = cy + dy;
               if (xx < 0 || xx >= input.width || yy < 0 || yy >= input.height)
               {
                  xx = cx;
                  yy = cy;
               }
               
               get_pixel(input, xx, yy, &r, &g, &b);
               sobelx += gx[dx + 1][dy + 1] * r;
               sobely += gy[dx + 1][dy + 1] * r;
            }
         }

         int color = sqrt(sobelx * sobelx + sobely * sobely);
         set_pixel(out_image, cx, cy, color, color, color);
      }
   }

   return out_image;
}

void process(Image input, const char *input_file_path)
{
   Image lum = calc_luminance(input);

   const char *output_file_path = add_infix(input_file_path, ".lum");
   printf("Saving %s\n", output_file_path);
   write_png_file(output_file_path, lum);

   Image sobel = calc_sobel(lum);
   output_file_path = add_infix(input_file_path, ".sobel");
   printf("Saving %s\n", output_file_path);
   write_png_file(output_file_path, sobel);

   free_image(lum);
   free_image(sobel);
}

int main(int argc, char **argv)
{
   srand(time(NULL));

   const char *program = args_shift(&argc, &argv);

   if (argc <= 0)
   {
      fprintf(stderr, "Usage: %s <input.png>\n", program);
      fprintf(stderr, "ERROR: no input file is provided\n");
      return 1;
   }

   const char *input_file_path = args_shift(&argc, &argv);

   if (!is_png(input_file_path))
   {
      fprintf(stderr, "ERROR: %s is not a PNG file\n", input_file_path);
      return 1;
   }   

   printf("Reading %s\n", input_file_path);
   Image i = read_png_file(input_file_path);
   printf("Dimensions: %d x %d\n", i.width, i.height);

   if (i.bit_depth != 8)
   {
      fprintf(stderr, "Only 8 bit images are supported\n");
      free_image(i);
      abort();
   }

   process(i, input_file_path);

   free_image(i);
   return 0;
}
