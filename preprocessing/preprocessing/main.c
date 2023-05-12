#include "pretreatment.h"

int main(int argc, char** argv)
{
    if (argc > 3)
    errx(EXIT_FAILURE, "Usage: image-file");

    // Initialise SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
      { 
        printf("Error initializing SDL2: %s\n", SDL_GetError());
        return 1;
      }

  char *filepath = argv[1];
  SDL_Surface *surface = IMG_Load(filepath);
  
  //Converting the surface to a new format to ensure that it is compatible with the display hardware
  //and the current rendering context, and that it can be efficiently drawn to the screen.
  surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);

  //Enhance Contrast
  enhance_contrast(surface);
  IMG_SavePNG(surface, "res/contrast.png");
  
  //Grayscale Filter
  grayscale(surface);
  IMG_SavePNG(surface, "res/grayscale.png");

  //Gaussian Blur
  gaussian_blur(surface, 3, 1.5);
  IMG_SavePNG(surface, "res/gaussian_blur.png");

  //Invert Filter
  surface = invert(surface);
  IMG_SavePNG(surface, "res/invert.png");

  //Dilate Filter
  dilate(surface);
  IMG_SavePNG(surface, "res/dilate.png");
  
  return 0;
}
