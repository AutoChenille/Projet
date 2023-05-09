#include "pretreatment.h"


SDL_Surface* threshold(SDL_Surface *input, Uint8 threshold_value)
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

            // Convert to grayscale using the average of the three color channels
            Uint8 grayscale = (r + g + b) / 3;
            
            // Thresholding
            if (grayscale > threshold_value) {
                pixels[y * width + x] = SDL_MapRGB(output->format, 255, 255, 255);
            } else {
                pixels[y * width + x] = SDL_MapRGB(output->format, 0, 0, 0);
            }
        }
    }

    SDL_UnlockSurface(output);
    return output;
}
