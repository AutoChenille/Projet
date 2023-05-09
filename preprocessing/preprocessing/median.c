#include "pretreatment.h"

int compare(const void *a, const void *b) {
    return (*(Uint8 *)a - *(Uint8 *)b);
}

Uint8 median(const Uint8 *values, int length) {
    Uint8 sorted_values[length];
    memcpy(sorted_values, values, length);

    qsort(sorted_values, length, sizeof(Uint8), compare);
    return sorted_values[length / 2];
}

void median_filter(SDL_Surface *surface, int window_size) {
    if (surface == NULL) {
        printf("Invalid surface.\n");
        return;
    }

    SDL_LockSurface(surface);

    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int half_window = window_size / 2;

    SDL_Surface *filtered_surface = SDL_ConvertSurface(surface, surface->format, 0);
    Uint32 *filtered_pixels = (Uint32 *)filtered_surface->pixels;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 window_values[window_size * window_size];
            int count = 0;

            for (int i = -half_window; i <= half_window; i++) {
                for (int j = -half_window; j <= half_window; j++) {
                    int y_neighbour = y + i;
                    int x_neighbour = x + j;

                    if (y_neighbour >= 0 && y_neighbour < height && x_neighbour >= 0 && x_neighbour < width) {
                        Uint32 pixel = pixels[y_neighbour * width + x_neighbour];
                        Uint8 r, g, b;
                        SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                        Uint8 gray = 0.299 * r + 0.587 * g + 0.114 * b;
                        window_values[count++] = gray;
                    }
                }
            }

            Uint8 value = median(window_values, count);
            filtered_pixels[y * width + x] = SDL_MapRGB(surface->format, value, value, value);
        }
    }

    memcpy(surface->pixels, filtered_surface->pixels, surface->pitch * surface->h);
    SDL_FreeSurface(filtered_surface);
    SDL_UnlockSurface(surface);
}
