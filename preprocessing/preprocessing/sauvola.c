#include "pretreatment.h"

double mean(const Uint8 *values, int length)
{
    double sum = 0;
    for (int i = 0; i < length; i++) {
        sum += values[i];
    }
    return sum / length;
}

double standard_deviation(const Uint8 *values, int length, double mean_value)
{
    double sum = 0;
    for (int i = 0; i < length; i++)
      {
        double diff = values[i] - mean_value;
        sum += diff * diff;
      }
    return sqrt(sum / length);
}

void sauvola(SDL_Surface *surface, int window_size, double k)
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
    int half_window = window_size / 2;

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

            double mean_value = mean(window_values, count);
            double std_dev = standard_deviation(window_values, count, mean_value);
            double threshold = mean_value * (1 + k * (std_dev / 128 - 1));

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
