#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "libtiles.h"

int main(int argc, char **argv)
{
  int opt, image_flag, output_flag, dir_flag;
  char *image, *dir, *output;

  image_flag = 0;
  output_flag = 0;
  dir_flag = 0;

  // Recebe as opções -i -o -p conforme solicitado
  opterr = 0;
  while ((opt = getopt(argc, argv, "i:o:p:h:")) != -1)
    switch (opt)
    {
    case 'i':
      image = optarg;
      image_flag = 1;
      break;
    case 'o':
      output = optarg;
      output_flag = 1;
      break;
    case 'p':
      dir = optarg;
      dir_flag = 1;
      break;
    case 'h':
      fprintf(stderr, "filename: %s\n", optarg);
      break;
    case '?':
      fprintf(stderr, "unknown option\n");
      exit(1);
    default:
      break;
    }

  if (!image_flag)
  {
    image = string_allocation(256);
    strcpy(image, "/dev/stdin");
  }
  if (!output_flag)
  {
    output = string_allocation(256);
    strcpy(output, "/dev/stdout");
  }
  if (!dir_flag)
  {
    dir = string_allocation(256);
    strcpy(dir, "./tiles/");
  }
 
  fprintf(stderr, "Abrindo imagem %s\n", image );
  image_ppm *image_in;
  image_in = open_image(image);
  fprintf(stderr, "Imagem Lida! %s\n", image );

  tiles_array *tiles;
  tiles = load_tiles(dir);
  fprintf(stderr, "\nCarregando pastilhas do dir %s\n", dir );

  fprintf(stderr, "\nCalculando media do bloco de cor das pastilhas do dir %s\n", dir );
  
  for( int n = 0; n < tiles->size; n++ )
    rgb_average_tiles(tiles, 0, 0, tiles->array[n].height, tiles->array[n].width, n);

  fprintf(stderr, "\nComparando %s com pastilhas do dir %s\n", image, dir );
  image_ppm *final_image;
  final_image = compare(image_in, tiles);

  fprintf(stderr, "\nCriando imagem final %s...\n", output );
  create_image_file(final_image, output);

  free(tiles);
  // free(tiles->array);
  free(image_in);
  // free(image_in->matrix);
  // free(image_in->pixel_average_color);
  free(final_image);
  // free(final_image->matrix);
  // free(final_image->pixel_average_color);

  return 0;
}