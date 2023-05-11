#include "pretreatment.h"

SDL_Surface* invert(SDL_Surface *input)
{
    if (input == NULL) {
        printf("Invalid surface.\n");
        return NULL;
    }

    SDL_Surface* output = SDL_ConvertSurfaceFormat(input, input->format->format, 0);
    if (output == NULL) {
        printf("Failed to create output surface: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_LockSurface(output);
    Uint32 *pixels = (Uint32 *)output->pixels;

    int width = output->w;
    int height = output->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y * width + x];
            SDL_GetRGB(pixel, output->format, &r, &g, &b);

            // Invert the color channels
            r = 255 - r;
            g = 255 - g;
            b = 255 - b;

            pixels[y * width + x] = SDL_MapRGB(output->format, r, g, b);
        }
    }

    SDL_UnlockSurface(output);
    return output;
}
