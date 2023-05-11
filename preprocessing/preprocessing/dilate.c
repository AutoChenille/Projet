#include "pretreatment.h"

SDL_Surface* dilate(SDL_Surface* input, int kernel_size) {
    if (input == NULL) {
        printf("Invalid surface.\n");
        return NULL;
    }

    int width = input->w;
    int height = input->h;

    SDL_Surface* output = SDL_CreateRGBSurface(0, width, height, input->format->BitsPerPixel, input->format->Rmask, input->format->Gmask, input->format->Bmask, input->format->Amask);

    if (output == NULL) {
        printf("Failed to create output surface: %s\n", SDL_GetError());
        return NULL;
    }

    int k = kernel_size / 2;

    SDL_LockSurface(input);
    SDL_LockSurface(output);

    Uint32* input_pixels = (Uint32*)input->pixels;
    Uint32* output_pixels = (Uint32*)output->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int is_black = 1;

            for (int dy = -k; dy <= k && is_black; dy++) {
                for (int dx = -k; dx <= k && is_black; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Uint32 pixel = input_pixels[ny * width + nx];
                        Uint8 r, g, b, a;
                        SDL_GetRGBA(pixel, input->format, &r, &g, &b, &a);

                        if (r == 255 && g == 255 && b == 255) {
                            is_black = 0;
                        }
                    }
                }
            }

            output_pixels[y * width + x] = is_black ? SDL_MapRGBA(output->format, 0, 0, 0, 255) : SDL_MapRGBA(output->format, 255, 255, 255, 255);
        }
    }

    SDL_UnlockSurface(input);
    SDL_UnlockSurface(output);

    return output;
}
