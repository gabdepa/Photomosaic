#ifndef __MOSAICO__
#define __MOSAICO__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

#define DIMENSION 2
#define RGB 255
#define MAX_ARRAY 2048


typedef struct pixel
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} pixel; // Tipo Pixel, suporta tipo RGB

typedef struct image_ppm
{
  char *type;
  int width;
  int height;
  int max_rgb;
  pixel pixel_average_color;
  pixel **matrix;

} image_ppm; // Tipo imagem PPM

typedef struct tiles_array
{
  int size;
  image_ppm *array;

} tiles_array; // Vetor para alocação de todas as imagens lidas

image_ppm *image_allocation(int size);

tiles_array *tiles_array_allocation(int size);

char *string_allocation(int size);

pixel **matrix_allocation(int width, int height);

void comments(FILE *file);

void rgb_average_image(image_ppm *image, int k, int l, int final_lin, int final_col);

void rgb_average_tiles(tiles_array *tile, int k, int l, int final_lin, int final_col);

void read_image_rgb(FILE *file, image_ppm *image);

image_ppm *open_image(char *filename);

void open_tile(char *filename, tiles_array *array_images);

tiles_array *load_tiles(char *directory);

float delta_c_calculation(pixel media_tile, pixel media_image);

void color_distance(image_ppm *image_out, image_ppm *image, tiles_array *tile, int line_ini, int col_ini, int line_final, int col_final);

image_ppm *compare(image_ppm *image, tiles_array *tiles);

void create_image_file(image_ppm *image_out, char *filename);

#endif