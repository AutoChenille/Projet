#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pretreatment.h"

SDL_Surface *rotate(SDL_Surface *surface, double degree)
{
  /* Takes a surface and a angle (in degrees) as parameters 
     and rotates the surface counter-clockwise. */

  //Lock the surface
  SDL_LockSurface(surface);

  int width = surface->w;
  int height = surface->h;

  //Create a copy of the surface
  SDL_Surface *copy = SDL_CreateRGBSurface(0,width, height, 32, 0, 0, 0, 0);

  //Create a new surface in SDL_PIXELFORMAT_RGB888
  SDL_Surface *new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);

  //Lock the surfaces
  SDL_LockSurface(copy);
  SDL_LockSurface(new_surface);

  //Get the pixels
  Uint32* copy_pixels = copy->pixels;
  Uint32* new_pixels = new_surface->pixels;

  //Convert the angle in degree to angle in radian
  double angle = degree * (M_PI / 180.0);

  double center_x = (width/(double)2);
  double center_y = (height/(double)2);

  //Rotation of all pixels (one by one)
  for (int x = 0; x < width; x++)
    {
      for (int y = 0; y < height; y++)
        {
	  double rx = (x - center_x) * cos(angle) - (y - center_y) * sin(angle) + center_x;
	  double ry =  (x - center_x) * sin(angle) + (y - center_y) * cos(angle) + center_y;
	  
	  if (0 <= rx && rx < new_surface->w && 0 <= ry && ry < new_surface->h)
            {
	      Uint8 r,g,b;
	      SDL_GetRGB(new_pixels[((int)ry)*(new_surface->w) + (int)rx], new_surface->format, &r, &g,&b); 
	      copy_pixels[((int)y)*(new_surface->w) + (int)x] = SDL_MapRGB(new_surface->format, r,g,b);
            }
	  else
            {
	      copy_pixels[((int)y)*(new_surface->w) + (int)x] = SDL_MapRGB(new_surface->format, 0,0,0);
            }
	  
        }
      
    }

  //Unlock the surfaces
  SDL_UnlockSurface(copy);
  SDL_UnlockSurface(surface);
  
  return copy;
}
