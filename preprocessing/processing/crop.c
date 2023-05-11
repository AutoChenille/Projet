#include "../preprocessing/pretreatment.h"

SDL_Surface* cropSurface(SDL_Surface* src, int x1, int y1, int x2, int y2)
{
    int width = x2 - x1;
    int height = y2 - y1;

    // Make sure the requested area is within the source surface
    if (x1 < 0 || y1 < 0 || x2 > src->w || y2 > src->h || width <= 0 || height <= 0)
      return NULL;

    SDL_Surface* dest = SDL_CreateRGBSurfaceWithFormat(0, width, height, src->format->BitsPerPixel, src->format->format);
    if (dest == NULL)
        return NULL;

    SDL_Rect srcRect;
    srcRect.x = x1;
    srcRect.y = y1;
    srcRect.w = width;
    srcRect.h = height;

    if (SDL_BlitSurface(src, &srcRect, dest, NULL) < 0)
      {
        SDL_FreeSurface(dest);
        return NULL;
      }

    return dest;
}
