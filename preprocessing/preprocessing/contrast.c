#include "pretreatment.h"

void enhance_contrast(SDL_Surface *surface)
{
    if (surface == NULL) {
        printf("Invalid surface.\n");
        return;
    }

    SDL_LockSurface(surface);

    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint8 min_r = 255, min_g = 255, min_b = 255;
    Uint8 max_r = 0, max_g = 0, max_b = 0;

    // Find the minimum and maximum pixel values
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            min_r = (r < min_r) ? r : min_r;
            min_g = (g < min_g) ? g : min_g;
            min_b = (b < min_b) ? b : min_b;

            max_r = (r > max_r) ? r : max_r;
            max_g = (g > max_g) ? g : max_g;
            max_b = (b > max_b) ? b : max_b;
        }
    }

    // Apply linear contrast stretching
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            Uint8 new_r = 255 * (r - min_r) / (max_r - min_r);
            Uint8 new_g = 255 * (g - min_g) / (max_g - min_g);
            Uint8 new_b = 255 * (b - min_b) / (max_b - min_b);

            pixels[y * width + x] = SDL_MapRGB(surface->format, new_r, new_g, new_b);
        }
    }

    SDL_UnlockSurface(surface);
}
