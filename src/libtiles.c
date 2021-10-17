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

// Função que aloca espaço em memória para um tipo ponteiro para image_ppm
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

// Função que aloca espaço em memória para um tipo ponteiro para tiles_array
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

// Função que aloca espaço em memória para um tipo ponteiro para char, ou seja, string
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

// Função que aloca espaço em memória para um tipo ponteiro para uma matrix do tipo pixel, tipo o qual contém 3 componentes, RGB
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

// Função que aloca espaço em memória para um tipo ponteiro para um tipo pixel
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

// Função que verifica por comentários na imagem
void comments(FILE *file)
{
  char hashbang;
  hashbang=getc(file);

  // Enquanto getc retornar um "#" ou "\n", continua obtendo o próximo caracter
  while ((hashbang =='#') || (hashbang == '\n'))
  {
    // Na Linha do comentário, enquanto não encontrar o \n, continua obtendo o próximo char do comentário
    while (hashbang != '\n')
      hashbang=getc(file);
    hashbang=getc(file);
  }
  ungetc(hashbang,file);
}

// Calcula a media RGB para uma imagem, com tamanho definido pelos parâmetros
void rgb_average_image(image_ppm *image, int k, int l, int final_lin, int final_col)
{
  image->pixel_average_color = RGB_pixel_allocation(1);

  int sum_red, sum_blue, sum_green, red, green, blue;
  sum_blue = 0;
  sum_green = 0;
  sum_red = 0;
  
  for (int i = k; i < final_lin; i++)
  {
    for (int j = l; j < final_col; j++)
    {
      red = image->matrix[i][j].red;
      green = image->matrix[i][j].green;
      blue = image->matrix[i][j].blue;

      // Obtém a soma de todos os valores para cada componente RGB
      sum_red = sum_red + red;
      sum_green = sum_green + green;
      sum_blue = sum_blue + blue;
    }
  }
  // Calcula média para o tamanho do bloco especificado e guardo no atributo de cor media do tipo image_ppm
  image->pixel_average_color->red = sum_red / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  image->pixel_average_color->green = sum_green / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  image->pixel_average_color->blue = sum_blue / ((final_col - l) * (final_lin - k) * CORRECTION_IMAGE);
  
}

// Calcula a media RGB para uma pastilha do vetor de pastilhas, com tamanho definido pelos parâmetros
void rgb_average_tiles(tiles_array *tile, int k, int l, int final_lin, int final_col, int n)
{
  int sum_red, sum_blue, sum_green, red, green, blue;
  sum_red = 0;
  sum_green = 0;
  sum_blue = 0;  

  for (int i = k; i < final_lin; i++)
  {
    for (int j = l; j < final_col; j++)
    {
      red = tile->array[n].matrix[i][j].red;
      green = tile->array[n].matrix[i][j].green;
      blue = tile->array[n].matrix[i][j].blue;

      // Obtém a soma de todos os valores para cada componente RGB
      sum_red = sum_red + red;
      sum_green = sum_green + green;
      sum_blue = sum_blue + blue;
    }
  }
  // Calcula média para o tamanho do bloco especificado e guardo no atributo de cor media da posição n do vetor de pastilhas
  tile->array[n].pixel_average_color->red = sum_red / (final_col * final_lin * CORRECTION_TILE);
  tile->array[n].pixel_average_color->green = sum_green / (final_col * final_lin * CORRECTION_TILE);
  tile->array[n].pixel_average_color->blue = sum_blue / (final_col * final_lin * CORRECTION_TILE);
}

// Função que lê os componentes RGB de uma imagem do tipo image_ppm
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

// Função que abre um arquivo e lê para gerar um output do tipo image_ppm
image_ppm *open_image(char *filename)
{
  // Inicializa e aloca espaço de memória para o tipo image_ppm
  FILE *file;
  image_ppm *image;
  image = image_allocation(1);
  image->type = string_allocation(3);

  // Tenta abrir o arquivo readonly, se não conseguir encerra o programa
  file = fopen(filename, "r");
  if (!file)
  {
    perror("Erro ao abrir arquivo da imagem input\n");
    exit(1);
  }

  // Lê o tipo da imagem do arquivo aberto e guarda o valor dentro do componente type do tipo image_ppm
  if (!fgets(image->type, 3, file)) // Verifica Tipo da imagem
  {
    perror("Erro ao utilizar fgets na imagem input.\n");
    exit(1);
  }
  // Verifica por comentários
  comments(file);

  // Lê a respectiva largura e altura da image e guarda no atribuito width e height, respectivamente e compara se leu 2 números
  if (fscanf(file, "%d %d", &image->width, &image->height) != DIMENSION)
  {
    perror("Erro ao ler dimensões da imagem input.\n");
    exit(1);
  }
  // Verifica por comentários
  comments(file);

  // Lê o índice Máximo RGB da imagen e guarda no atributo max_rgb do tipo image_ppm
  if (fscanf(file, "%d", &image->max_rgb) != 1)
  {
    perror("Erro ao ler índice RGB da imgem input.\n");
    exit(1);
  }
  // Verifica por comentários
  comments(file);

  // Aloca espaço para o atributo matrix do tipo image_ppm com a largura e altura lida anteriormente
  image->matrix = matrix_allocation(image->width, image->height);

  // Verifica se a imagem é do tipo P6, caso seja utiliza o fread para que leia a matrix e guarde a partir do indice 0 da altura
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
  // Caso seja P3, utiliza a função para ler conforme instruído anteriormente

  fclose(file);
  return image;
}

// Função que abre um arquivo e lê para gerar atributos para a posição atual do vetor do tipo image_ppm
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

  // Guarda largura e altura da imagem
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

  // Variável utilizada para melhor legibilidade do código
  int width = array_images->array[array_images->size].width;
  int height = array_images->array[array_images->size].height;

  // Aloca espaço para a matrix do tipo image_ppm da posição especificado do vetor de pastilhas
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

// Função que lê as pastilhas do diretório e carrega-as para um vetor de pastilhas 
tiles_array *load_tiles(char *directory)
{
  // Alocação e inicialização dos atributos do vetor de pastilhas
  tiles_array *array_images;
  array_images = tiles_array_allocation(1);
  array_images->array = image_allocation(MAX_ARRAY);
  array_images->size = 0;

  DIR *dir_stream;
  struct dirent *dir_entry;
  char *filename;
  int mult;
  mult = 1;

  // Aloca espaço para receber o nome do arquivo
  filename = string_allocation(1024);
  // Abre diretório para dentro da variável dir_stream
  dir_stream = opendir(directory);
  if (!dir_stream)
  {
    perror("Erro ao abrir diretório de pastilhas.\n");
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
    // Copia o nome do diretório passado como parâmetro para dentro da variável filename 
    strcpy(filename, directory);
    // Concatena o nome do arquivo atual lido com o nome atribuido anteriormente a variável filename
    strcat(filename, dir_entry->d_name);

    // Aloca Espaço para o tipo e para a o valor médio dos componentes RGB da imagem
    array_images->array[array_images->size].type = string_allocation(3);
    array_images->array[array_images->size].pixel_average_color = RGB_pixel_allocation(1);
    // Função que abre a pastilha e le para o vetor de pastilhas
    open_tile(filename, array_images);
    // Incrementa o tamanho do vetor
    array_images->size++;
    
    // Caso o tamanho do array esteja maior do que o espaço previamente alocado, realiza uma realocação de memória para o vetor de imagens
    if (array_images->size >= MAX_ARRAY * mult)
    {
      mult++;
      array_images->array = realloc(array_images->array, sizeof(image_ppm) * (MAX_ARRAY * mult));
    }
  }
  (void)closedir(dir_stream);
  return array_images;
}

// Calcula o Delta C para averiguar a cor mais próxima
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
  // Inicializa o menor delta c encontrado com a primeira imagem do vetor de pastilhas comparada com a imagem
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

// Função que realiza a comparação dos blocos de mesmo tamanho da imagem com o da pastilha
image_ppm *compare(image_ppm *image, tiles_array *tiles)
{
  // Inicializa informações para o arquivo de saída image_out
  image_ppm *image_out;
  image_out = image_allocation(1);
  image_out->type = string_allocation(3);
  image_out->matrix = matrix_allocation(image->width, image->height);

  // Copia os atributos da image original para a imagem final
  strcpy(image_out->type, image->type);
  image_out->width = image->width;
  image_out->height = image->height;
  image_out->max_rgb = image->max_rgb;

  // Variaveis utilizadas para melhor legibilidade
  int height_tiles = tiles->array[0].height;
  int width_tiles = tiles->array[0].width;

  // Percorre as linhas e colunas da imagem em um incremento de tamanho da pastilha
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

// Função que cria o arquivo final com base no output da função compare
void create_image_file(image_ppm *image_out, char *filename)
{
  // Inicializa um tipo arquivo para escrita
  FILE *file;
  file = fopen(filename, "w");
  if (!file)
  {
    perror("Erro ao criar arquivo.\n");
    exit(1);
  }

  // Preenche conforme formato PPM os atributos da imagem
  fprintf(file, "%s\n", image_out->type);
  fprintf(file, "#@gabdepa\n");
  fprintf(file, "%d %d\n", image_out->width, image_out->height);
  fprintf(file, "%d\n", image_out->max_rgb);

  // Caso o tipo da imagem for P3, escreve pixel por pixel, na ordem RGB
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
  // Caso a imagem seja P6, escreve a partir do índice 0 da altura com largura*3, por ser um tipo RGB e altura da imagem
  fclose(file);
}