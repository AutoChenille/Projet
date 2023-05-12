#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <limits.h>
#include "pretreatment.h"

double sobelx[3][3] = {{-1.0, 0.0, 1.0},
		       {-2.0, 0.0, 2.0},
		       {-1.0, 0.0, 1.0}};

double sobely[3][3] = {{-1.0, -2.0, -1.0},
		       {0.0, 0.0, 0.0},
		       {1.0, 2.0, 1.0}};

double gradient(SDL_Surface *surface, double kernel[3][3], int row, int col)
{
  int width = surface->w;
  int height = surface->h;
  Uint32 *pixels = surface->pixels;
  double res = 0;
  
  for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
        {
	  int x = i + row;
	  int y = j + col;
	  if (x >= 0 && y >= 0 && x < width && y < height)
            {
	      Uint8 r,g,b;
	      SDL_GetRGB(pixels[y*(width) + x], surface->format, &r,&g,&b);
	      res += r * kernel[i][j];
            }
        }
    }
  
  return res;
}

void sobel_filter(SDL_Surface *surface)
{
  //Lock the surface
  SDL_LockSurface(surface);

  //Get the width, heigth and pixels of the selected surface
  int width = surface->w;
  int height = surface->h;
  
  Uint32 *pixels = surface->pixels;
  
  //Variable for SobelX -> gx / SobelY gy / SobelX+Y -> gradient
  double gx, gy;
  double grad;
  
  //Filter all the pixels one by one
  for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
        {
	  gx = gradient(surface, sobelx, j, i);
	  gy = gradient(surface, sobely, j, i);
	  grad = sqrt(gx * gx + gy * gy);
	  
	  if (grad > 128)
	    {
	      pixels[i*(width) + j] = SDL_MapRGB(surface->format, 255, 255, 255);
	    }
	  else
	    {
	      pixels[i*(width) + j] = SDL_MapRGB(surface->format, 0, 0, 0);
	    }
        }
    }

  //Unlock the surface
  SDL_UnlockSurface(surface);
}

//Same function from the TP06.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format)
{
  /* Takes a pixel and a surface's format and convert the pixel into grayscale pixel */
  Uint8 r, g, b;

  SDL_GetRGB(pixel_color, format, &r, &g, &b);

  Uint8 average = (0.3 * r) +  (0.59 * g) + (0.11 * b);

  r = g = b = average;

  Uint32 color = SDL_MapRGB(format, r, g, b);

  return color;
}

//Same function from the TP06.
void grayscale(SDL_Surface *surface)
{
  /* Takes a surface as a parameter and grayscale all the pixels of the surface */
  Uint32 *pixels = surface->pixels;
  int len = surface->w * surface->h;
  SDL_PixelFormat *format = surface->format;
  int i = 0;

  SDL_LockSurface(surface);

  while (i < len)
    {
      pixels[i] = pixel_to_grayscale(pixels[i], format);
      i++;
    }

  SDL_UnlockSurface(surface);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;
        default:
            return 0;
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16 *)p = pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

void blur(SDL_Surface *surface, int strength)
{
  /* Blurs a surface, strength is the blur's strength */
  
  // Allocate a temporary surface to store the blurred version
  SDL_Surface *tempSurface = SDL_CreateRGBSurface(0, surface->w, surface->h, 24, 0, 0, 0, 0);
  
  // Loop over every pixel in the surface
  for (int y = 0; y < surface->h; y++)
    {
      for (int x = 0; x < surface->w; x++)
	{
	  // Calculate the average color of the surrounding pixels
	  int red = 0, green = 0, blue = 0;
	  int count = 0;
	  for (int dy = -strength; dy <= strength; dy++)
	    {
	      for (int dx = -strength; dx <= strength; dx++)
		{
		  int nx = x + dx;
		  int ny = y + dy;
		  if (nx >= 0 && nx < surface->w && ny >= 0 && ny < surface->h)
		    {
		      Uint32 pixel = getpixel(surface, nx, ny);
		      red += (pixel >> 16) & 0xff;
		      green += (pixel >> 8) & 0xff;
		      blue += pixel & 0xff;
		      count++;
		    }
		}
	    }
	  red /= count;
	  green /= count;
	  blue /= count;
	  
	  // Set the pixel in the temporary surface
	  Uint32 pixel = (red << 16) | (green << 8) | blue;
	  putpixel(tempSurface, x, y, pixel);
	}
    }
  
  // Copy the blurred surface back to the original surface
  SDL_BlitSurface(tempSurface, NULL, surface, NULL);
  
  // Free the temporary surface
  SDL_FreeSurface(tempSurface);
}

// calculate the mean and standard deviation of a window of pixels
void calculate_mean_and_stddev(Uint8 *pixels, int pitch, int x, int y, int window_size, double *mean, double *stddev)
{
  // initialize the sum of pixel values and the sum of squared pixel values
  double sum = 0;
  double sum_squared = 0;
  
  // iterate over the pixels in the window
  for (int i = x; i < x + window_size; i++)
  {
      for (int j = y; j < y + window_size; j++)
      {
	  // calculate the index of the pixel
	  int index = i * pitch + j * 4;
	  
	  // add the pixel value to the sum
	  sum += pixels[index];
	  
	  // add the squared pixel value to the sum_squared
	  sum_squared += pow(pixels[index], 2);
        }
    }
  
  // calculate the mean of the pixel values
  *mean = sum / (window_size * window_size);
  
  // calculate the standard deviation of the pixel values
  double variance = sum_squared / (window_size * window_size) - pow(*mean, 2);
  *stddev = sqrt(variance);
}

