#include "pretreatment.h"

// Function to get pixel intensity (assuming grayscale image)
Uint8 get_pixel_intensity(SDL_Surface* surface, int x, int y)
{
    Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

    // Return the average of RGB channels
    return (r + g + b) / 3;
}

// Function to apply adaptive thresholding
SDL_Surface* adaptive_threshold(SDL_Surface* surface, int neighborhood_size, int C)
{
    // Convert the surface to a new one to avoid modifying the original
    SDL_Surface* new_surface = SDL_ConvertSurface(surface, surface->format, 0);

    for (int y = 0; y < new_surface->h; y++) {
        for (int x = 0; x < new_surface->w; x++) {
            // Calculate local mean
            int total = 0;
            int count = 0;
            for (int i = -neighborhood_size; i <= neighborhood_size; i++) {
                for (int j = -neighborhood_size; j <= neighborhood_size; j++) {
                    int new_x = x + i;
                    int new_y = y + j;

                    // Check if new_x and new_y are inside the surface
                    if (new_x >= 0 && new_x < new_surface->w && new_y >= 0 && new_y < new_surface->h) {
                        total += get_pixel_intensity(surface, new_x, new_y);
                        count++;
                    }
                }
            }
            
            int mean = total / count;
            Uint8 intensity = get_pixel_intensity(surface, x, y);
            Uint32 new_pixel;

            // Apply thresholding
            if (intensity < mean - C) {
                new_pixel = SDL_MapRGB(new_surface->format, 0, 0, 0);  // black
            } else {
                new_pixel = SDL_MapRGB(new_surface->format, 255, 255, 255);  // white
            }

            ((Uint32*)new_surface->pixels)[y * new_surface->w + x] = new_pixel;
        }
    }

    return new_surface;
}
