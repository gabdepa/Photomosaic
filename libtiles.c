#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "libtiles.h"

#define DIMENSION 2
#define RGB 255
#define MAX_ARRAY 2048
#define CORRECTION_TILE 1
#define CORRECTION_IMAGE 1


image_ppm *image_allocation(int size)
{
  image_ppm *image;
  image = malloc(size * sizeof(image_ppm));
  if (!image)
  {
    perror("Erro ao alocar imagem\n");
    exit(1);
  }
  return image;
}

tiles_array *tiles_array_allocation(int size)
{
  tiles_array *tiles;
  tiles = malloc(size * sizeof(tiles_array));
  if (!tiles)
  {
    perror("Erro ao alocar vetor de pastilhas\n");
    exit(1);
  }
  return tiles;
}

char *string_allocation(int size)
{
  char *type;
  type = malloc(size * sizeof(char));
  if (!type)
  {
    perror("Erro ao alocar string");
    exit(1);
  }
  return type;
}

pixel **matrix_allocation(int width, int height)
{
  pixel **grade;

  grade = malloc(height * sizeof(pixel *));
  if (!grade)
  {
    perror("Erro ao alocar matriz\n");
    exit(1);
  }
  grade[0] = malloc(height * width * sizeof(pixel));
  if (!grade[0])
  {
    perror("Erro ao alocar matriz\n");
    exit(1);
  }

  for (int i = 1; i < height; i++)
    grade[i] = grade[0] + i * width;
  return grade;
}

pixel *RGB_pixel_allocation(int size)
{
  pixel *grade;

  grade = malloc(size * sizeof(pixel));
  if (!grade)
  {
    perror("Erro ao alocar matriz\n");
    exit(1);
  }
  return grade;
}

void comments(FILE *file)
{
  char hashbang;
  hashbang=getc(file);

  while ((hashbang =='#') || (hashbang == '\n'))
  {
    while (hashbang != '\n')
      hashbang=getc(file);
    hashbang=getc(file);
  }
  ungetc(hashbang,file);
}

void rgb_average_image(image_ppm *image, int k, int l, int final_lin, int final_col)
{
  int sum_red, sum_blue, sum_green, red, green, blue;
  sum_blue = 0;
  sum_green = 0;
  sum_red = 0;

  for (int i = k; i < final_lin; i++)
    for (int j = l; j < final_col; j++)
    {
      red = image->matrix[i][j].red;
      green = image->matrix[i][j].green;
      blue = image->matrix[i][j].blue;

      sum_red = sum_red + red;
      sum_green = sum_green + green;
      sum_blue = sum_green + blue;
    }
  image->pixel_average_color->red = sum_red / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  image->pixel_average_color->green = sum_green / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  image->pixel_average_color->blue = sum_blue / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  
  FILE *file;
  file = fopen("media_pixel_IMAGE.txt", "w");
  if (!file)
  {
    perror("Erro ao criar arquivo.\n");
    exit(1);
  }
  fprintf(file, "Red:%i Green:%i Blue:%i\n", image->pixel_average_color->red, image->pixel_average_color->green, image->pixel_average_color->blue);
  fclose(file);

}

// CONTÉM ERRO NESSA FUNÇÃO
void rgb_average_tiles(tiles_array *tile, int k, int l, int final_lin, int final_col)
{
  int sum_red, sum_blue, sum_green, red, green, blue;
  sum_red = 0;
  sum_green = 0;
  sum_blue = 0;  

  // FILE *file;
  // file = fopen("media_pixel_TILES.txt", "w");
  // if (!file)
  // {
  //   perror("Erro ao criar arquivo.\n");
  //   exit(1);
  // }

  for (int n = 0; n < tile->size; n++)
  {
    for (int i = k; i < final_lin; i++)
    {
      for (int j = l; j < final_col; j++)
      {
        red = tile->array[n].matrix[i][j].red;
        green = tile->array[n].matrix[i][j].green;
        blue = tile->array[n].matrix[i][j].blue;

        sum_red = sum_red + red;
        sum_green = sum_green + green;
        sum_blue = sum_blue + blue;
      }
    }
    tile->array[n].pixel_average_color->red = sum_red / (final_col * final_lin * CORRECTION_TILE);
    tile->array[n].pixel_average_color->green = sum_green / (final_col * final_lin * CORRECTION_TILE);
    tile->array[n].pixel_average_color->blue = sum_blue / (final_col * final_lin * CORRECTION_TILE);
  
  // fprintf(file, "Red:%i Green:%i Blue:%i\n", tile->array[n].pixel_average_color.red, tile->array[n].pixel_average_color.green, tile->array[n].pixel_average_color.blue);
  }
  // fclose(file);
}

void read_image_rgb(FILE *file, image_ppm *image)
{
  int r, g, b;
  for (int i = 0; i < image->height; i++)
    for (int j = 0; j < image->width; j++)
    {
      if (fscanf(file, "%d %d %d ", &r, &g, &b) != 3)
      {
        perror("Erro ao ler matriz.\n");
        exit(1);
      }
      image->matrix[i][j].red = r;
      image->matrix[i][j].green = g;
      image->matrix[i][j].blue = b;
    }
}

image_ppm *open_image(char *filename)
{
  FILE *file;
  image_ppm *image;
  image = image_allocation(1);
  image->type = string_allocation(3);
  image->pixel_average_color = RGB_pixel_allocation(1);

  file = fopen(filename, "r");
  if (!file)
  {
    perror("Erro ao abrir arquivo da imagem input\n");
    exit(1);
  }

  if (!fgets(image->type, 3, file)) // Verifica Tipo da imagem
  {
    perror("Erro ao utilizar fgets na imagem input.\n");
    exit(1);
  }
  comments(file);

  if (fscanf(file, "%d %d", &image->width, &image->height) != DIMENSION)
  {
    perror("Erro ao ler dimensões da imagem input.\n");
    exit(1);
  }
  comments(file);

  if (fscanf(file, "%d", &image->max_rgb) != 1)
  {
    perror("Erro ao ler índice RGB da imgem input.\n");
    exit(1);
  }
  comments(file);
  image->matrix = matrix_allocation(image->width, image->height);

  if (strcmp(image->type, "P6") == 0)
  {
    if (fread(image->matrix[0], image->width * 3, image->height, file) != image->height)
    {
      perror("Erro ao ler matriz da imagem input.\n");
      exit(1);
    }
  }
  else
    read_image_rgb(file, image);

  fclose(file);
  return image;
}

void open_tile(char *filename, tiles_array *array_images)
{
  FILE *file;

  file = fopen(filename, "r");
  if (!file)
  {
    perror("Erro ao abrir arquivo de pastilhas.\n");
    exit(1);
  }

  if (! fgets(array_images->array[array_images->size].type, 3, file)) // Verifica Tipo da imagem
  {
    perror("Erro ao utilizar fgets no tipo da pastilha.\n");
    exit(1);
  }
  comments(file);

  if (fscanf(file, "%d %d", &array_images->array[array_images->size].width, &array_images->array[array_images->size].height) != DIMENSION)
  {
    perror("Erro ao ler dimensões da imagem da pastilha.\n");
    exit(1);
  }
  comments(file);

  if (fscanf(file, "%d", &array_images->array[array_images->size].max_rgb) != 1)
  {
    perror("Erro ao ler índice RGB da pastilha.\n");
    exit(1);
  }
  if (array_images->array[array_images->size].max_rgb != RGB)
  {
    perror("Erro no índice RGB da pastilha, diferente de 255.\n");
    exit(1);
  }

  int width = array_images->array[array_images->size].width;
  int height = array_images->array[array_images->size].height;
  array_images->array[array_images->size].matrix = matrix_allocation(width, height);
  if (strcmp(array_images->array[array_images->size].type, "P6") == 0)
  {
    if (fread(array_images->array[array_images->size].matrix[0], width * 3, height, file) != height )
    {
      perror("Erro ao ler matriz da pastilha.\n");
      exit(1);
    }
  }
  else
    read_image_rgb(file, &array_images->array[array_images->size]);
  
  fclose(file);
}

tiles_array *load_tiles(char *directory)
{
  tiles_array *array_images;
  array_images = tiles_array_allocation(1);
  array_images->size = 0;
  array_images->array = image_allocation(MAX_ARRAY);

  DIR *dir_stream;
  struct dirent *dir_entry;
  char *filename;
  int mult;
  mult = 1;

  filename = string_allocation(1024);
  dir_stream = opendir(directory);
  if (!dir_stream)
  {
    perror("Erro ao abrir diretório de pastilhas.\n");
    exit(1);
  }

  FILE *file;
  file = fopen("Filename_TILES.txt", "w");
  if (!file)
  {
    perror("Erro ao criar arquivo.\n");
    exit(1);
  }
  
  while (1)
  {
    dir_entry = readdir(dir_stream); // Pega a próxima entrada do diretório
    if (!dir_entry)                  // Se for NULL, encerra a varredura
      break;
    if (!strcmp(dir_entry->d_name, "."))
      continue;
    if (!strcmp(dir_entry->d_name, ".."))
      continue;
    strcpy(filename, directory);
    strcat(filename, dir_entry->d_name);

    array_images->array[array_images->size].type = string_allocation(3);
    array_images->array[array_images->size].pixel_average_color = RGB_pixel_allocation(1);
    open_tile(filename, array_images);
    fprintf(file, "Filename %s Posição: %i\n", filename, array_images->size);
    array_images->size++;
    if (array_images->size >= MAX_ARRAY * mult)
    {
      mult++;
      array_images->array = realloc(array_images->array, sizeof(image_ppm) * (MAX_ARRAY * mult));
    }
  }
  fclose(file);
  (void)closedir(dir_stream);
  return array_images;
}

float delta_c_calculation(pixel* media_tile, pixel* media_image)
{
  int red_delta, blue_delta, green_delta;
  float R, delta_c;
  
  red_delta = media_tile->red - media_image->red;
  green_delta = media_tile->green - media_image->green;
  blue_delta = media_tile->blue - media_image->blue;
  R = (media_image->red + media_tile->red) / 2;

  delta_c = sqrt( ((2 + (R/256))*pow(red_delta,2)) + (4*pow(green_delta,2)) + ((2 + ((255 - R)/256)) * pow(blue_delta,2)) );

  return delta_c;
}

void color_distance(image_ppm *image_out, image_ppm *image, tiles_array *tile, int line_ini, int col_ini, int line_final, int col_final)
{
  float delta_c, min_delta_c;
  min_delta_c = delta_c_calculation(tile->array[0].pixel_average_color, image->pixel_average_color);
  
  int tile_index = 0;
  // Calcula cor mais próxima e marca qual o índice no array de pastilhas da mesma
  for (int i = 1; i < tile->size; i++)
  {
    delta_c = delta_c_calculation(tile->array[i].pixel_average_color, image->pixel_average_color);    
    if (delta_c < min_delta_c)
    {
      min_delta_c = delta_c;
      tile_index = i;
    }
  }
  // Preenche imagem final com imagem da pastilha do mesmo tamanho
  int i = 0;
  for (int k = line_ini; k < line_final; k++)
  {
    int j = 0;
    for (int l = col_ini; l < col_final; l++)
    {
      image_out->matrix[k][l].red = tile->array[tile_index].matrix[i][j].red;
      image_out->matrix[k][l].green = tile->array[tile_index].matrix[i][j].green;
      image_out->matrix[k][l].blue = tile->array[tile_index].matrix[i][j].blue;
      j++;
    }
    i++;
  }
}

image_ppm *compare(image_ppm *image, tiles_array *tiles)
{
  // Inicializa informações para o arquivo de saída image_out
  image_ppm *image_out;
  image_out = image_allocation(1);
  image_out->type = string_allocation(3);
  image_out->matrix = matrix_allocation(image->width, image->height);

  strcpy(image_out->type, image->type);
  image_out->width = image->width;
  image_out->height = image->height;
  image_out->max_rgb = image->max_rgb;

  int height_tiles = tiles->array[0].height;
  int width_tiles = tiles->array[0].width;
  
  for (int i = 0; i < image_out->height; i += height_tiles)
  {
    for (int j = 0; j < image_out->width; j += width_tiles)
    {
      if ( (i+height_tiles > image->height) && (j+width_tiles > image->width) ) // Caso extremo, no qual o último bloco é menor e menos largo do que a pastilha
      {
        rgb_average_image(image, i, j, image->height, image->width);
        color_distance(image_out, image, tiles, i, j, image->height, image->width);
      }
      else if ( (i+height_tiles > image->height) && (j+width_tiles <= image->width) ) // Caso em que altura restante da imagem é menor que a altura da pastilha
      {
        rgb_average_image(image, i, j, image->height, j+width_tiles);
        color_distance(image_out, image, tiles, i, j, image->height, j+width_tiles);
      }
      else if ( (j+width_tiles > image->width) && (i+height_tiles <= image->height) ) // Caso em que largura restante da imagem é menor que a largura da pastilha
      {
        rgb_average_image(image, i, j, i+height_tiles, image->width);
        color_distance(image_out, image, tiles, i, j, i+height_tiles, image->width);
      }
      else //Caso normal
      {
        rgb_average_image(image, i, j, i+height_tiles, j+width_tiles);
        color_distance(image_out, image, tiles, i, j, i+height_tiles, j+width_tiles);
      }
    }
  }
  return image_out;
}

void create_image_file(image_ppm *image_out, char *filename)
{
  FILE *file;
  file = fopen(filename, "w");
  if (!file)
  {
    perror("Erro ao criar arquivo.\n");
    exit(1);
  }

  fprintf(file, "%s\n", image_out->type);
  fprintf(file, "#@gabdepa\n");
  fprintf(file, "%d %d\n", image_out->width, image_out->height);
  fprintf(file, "%d\n", image_out->max_rgb);
  if (strcmp(image_out->type, "P3") == 0)
  {
    for (int i = 0; i < image_out->height; i++)
      for (int j = 0; j < image_out->width; j++)
      {
        fprintf(file, "%i %i %i ", image_out->matrix[i][j].red, image_out->matrix[i][j].green, image_out->matrix[i][j].blue);
      }
  }
  else
    fwrite(image_out->matrix[0], image_out->width * 3, image_out->height, file);
  fclose(file);
}