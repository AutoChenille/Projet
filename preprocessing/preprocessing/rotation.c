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

/*
double detect_angle(int corner[4][2])
{
  // calculate the center point of the square
  int center_x = (corner[0][0] + corner[1][0] + corner[2][0] + corner[3][0]) / 4;
  int center_y = (corner[0][1] + corner[1][1] + corner[2][1] + corner[3][1]) / 4;

  // calculate the distance from the center point to each corner
  double distances[4];
  
  distances[0] = sqrt(pow(corner[0][0] - center_x, 2)
		      + pow(corner[0][1] - center_y, 2));
  distances[1] = sqrt(pow(corner[1][0] - center_x, 2)
		      + pow(corner[1][1] - center_y, 2));
  distances[2] = sqrt(pow(corner[2][0] - center_x, 2)
		      + pow(corner[2][1] - center_y, 2));
  distances[3] = sqrt(pow(corner[3][0] - center_x, 2)
		      + pow(corner[3][1] - center_y, 2));

  // find the two corners that are farthest apart
  int index1 = 0;
  int index2 = 0;
  double max_distance = 0;

  for (int i = 0; i < 4; i++)
  {
    if (distances[i] > max_distance)
    {
      max_distance = distances[i];
      index1 = i;
    }
  }

  max_distance = 0;

  for (int i = 0; i < 4; i++)
  {
    if (i != index1 && distances[i] > max_distance)
    {
      max_distance = distances[i];
      index2 = i;
    }
  }

  if (index2 == 0)
    index2 = 0;
  
  // calculate the angle between the two corners and the X-axis
  int dx = corner[1][0] - corner[0][0];
  int dy = corner[1][1] - corner[0][1];
  double angle = atan2(dy, dx) * 180 / M_PI;
  
  // adjust the angle if necessary
  if (angle < 0)
    angle += 180;

  return angle;
}
*/

double calculateSudokuRotation(int corners[4][2])
{
    // Calculate deltas for top and bottom edges
    double dxTop = corners[1][0] - corners[0][0];
    double dyTop = corners[1][1] - corners[0][1];
    double dxBottom = corners[3][0] - corners[2][0];
    double dyBottom = corners[3][1] - corners[2][1];

    // Calculate angles for top and bottom edges in radians
    double angleTop = atan2(dyTop, dxTop);
    double angleBottom = atan2(dyBottom, dxBottom);

    // Convert to degrees and adjust range from -180 to 180
    angleTop = angleTop * 180 / M_PI;
    angleBottom = angleBottom * 180 / M_PI;

    // Calculate and return average angle
    double averageAngle = (angleTop + angleBottom) / 2;
    return averageAngle;
}
