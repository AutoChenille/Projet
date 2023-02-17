#include <stdio.h>
#include <err.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pretreatment.h"

/*
SDL_Surface *perspective_correction(SDL_Surface *input, int src[4][2])
{
  int dst[4][2] = {{0, 0},
		   {, 0},
		   {},
		   {0, }};
  
}
*/

int str_to_int(char *str)
{
  int res = 0;
  int sign = 1;

  if (str[0] == '-')
    {
      sign = -1;
      str++;
    }

  while (*str != '\0')
    {
      res = res * 10 + *str - 48;
      str++;
    }

  return res * sign;
}

int main(int argc, char** argv)
{
  if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file");

  char *filepath = argv[1];
  SDL_Surface *surface = IMG_Load(filepath);
  int angle = str_to_int(argv[2]);

  //surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);

  surface = rotate(surface, (double)angle);
  IMG_SavePNG(surface, "rotated.png");

  blur(surface, 1);
  IMG_SavePNG(surface, "blur.png");

  //Converting the surface to a new format to ensure that it is compatible with the display hardware
  //and the current rendering context, and that it can be efficiently drawn to the screen.

  surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
  surface_to_grayscale(surface);
  IMG_SavePNG(surface, "grayscale.png");
  display("grayscale.png");
  sobel_filter(surface);
  IMG_SavePNG(surface, "sobelXY.png");

  //Perspective
  //SDL_Surface* output = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
  //int points[8] = {628, 179, 1366, 695, 848, 1435, 110, 917};
  float points[4][2] = {{628, 179},
			{1366, 695},
			{848, 1435},
			{110, 917}};
  //warpSudoku(surface, output, src);
  //SDL_Surface *output = perspective_correction(surface, points);
  //IMG_SavePNG(output, "output.png");

  //SDL_FreeSurface(output);
  SDL_FreeSurface(surface);
  
  return 0;
}
