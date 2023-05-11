#include "pretreatment.h"

int otsu_threshold(SDL_Surface *surface)
{
    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int histogram[256] = {0};

    for (int y = 0; y < height; y++)
      {
        for (int x = 0; x < width; x++)
	  {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = 0.299 * r + 0.587 * g + 0.114 * b;
            histogram[gray]++;
	  }
      }

    int total_pixels = width * height;
    double sum = 0;
    for (int i = 0; i < 256; i++)
        sum += i * histogram[i];

    double sumB = 0;
    int wB = 0;
    int wF = 0;
    double varMax = 0;
    int threshold = 0;

    for (int i = 0; i < 256; i++)
      {
        wB += histogram[i];
        if (wB == 0) continue;
        
        wF = total_pixels - wB;
        if (wF == 0) break;

        sumB += i * histogram[i];
        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax)
	  {
            varMax = varBetween;
            threshold = i;
	  }
      }

    return threshold;
}

void otsu_adaptive_threshold(SDL_Surface *surface)
{
    if (surface == NULL)
      {
        printf("Invalid surface.\n");
        return;
      }

    SDL_LockSurface(surface);

    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int threshold = otsu_threshold(surface);

    for (int y = 0; y < height; y++)
      {
        for (int x = 0; x < width; x++)
	  {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = 0.299 * r + 0.587 * g + 0.114 * b;
            Uint8 value = (gray > threshold) ? 255 : 0;
            pixels[y * width + x] = SDL_MapRGB(surface->format, value, value, value);
	  }
      }

    SDL_UnlockSurface(surface);
}
