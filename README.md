# Photomosaic

In this implemented code in C we take an image in PPM format(P3 or P6).

After we take as input a file containing tinny little images in ppm format too(P3 or P6),

Finally we create a Photomosaic using the tiles passed.

In Linux CLI, use:
gcc -Wall -Werror -lm -g libtiles.c mosaico.c -o mosaico

This will create an executable file called "mosaico"

The parameters are:
-i => input image, ex: "zelda.ppm"
-o => output image name disered, ex: "zelda_mosaic.ppm"
-p => dir cointaining the tiles, if not passed assume "./tiles", ex:"./tiles32/"
