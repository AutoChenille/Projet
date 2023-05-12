#include "pretreatment.h"

const double PI = 3.14159265358979323846;

double gaussian(double x, double sigma)
{
    return (1.0 / (sigma * sqrt(2.0 * PI))) * exp(-(x * x) / (2.0 * sigma * sigma));
}

void gaussian_blur(SDL_Surface *surface, int radius, double sigma)
{
    if (surface == NULL)
    {
        printf("Invalid surface.\n");
        return;
    }
    
    SDL_LockSurface(surface);

    Uint32 *pixels = (Uint32 *) surface->pixels;
    int width = surface->w;
    int height = surface->h;
    //int bpp = surface->format->BytesPerPixel;

    double *kernel = (double *) malloc((2 * radius + 1) * sizeof(double));
    for (int i = 0; i < 2 * radius + 1; i++) {
        kernel[i] = gaussian(i - radius, sigma);
    }

    Uint32 *temp = (Uint32 *) malloc(width * height * sizeof(Uint32));
    memcpy(temp, pixels, width * height * sizeof(Uint32));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double r_sum = 0, g_sum = 0, b_sum = 0, k_sum = 0;
            
            for (int i = -radius; i <= radius; i++) {
                int xi = x + i;
                if (xi >= 0 && xi < width) {
                    Uint32 pixel = temp[y * width + xi];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                    double k = kernel[radius + i];
                    r_sum += r * k;
                    g_sum += g * k;
                    b_sum += b * k;
                    k_sum += k;
                }
            }

            Uint8 r = round(r_sum / k_sum);
            Uint8 g = round(g_sum / k_sum);
            Uint8 b = round(b_sum / k_sum);
            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }

    memcpy(temp, pixels, width * height * sizeof(Uint32));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double r_sum = 0, g_sum = 0, b_sum = 0, k_sum = 0;
            
            for (int i = -radius; i <= radius; i++) {
                int yi = y + i;
                if (yi >= 0 && yi < height) {
                    Uint32 pixel = temp[yi * width + x];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                    double k = kernel[radius + i];
                    r_sum += r * k;
                    g_sum += g * k;
                    b_sum += b * k;
                    k_sum += k;
                }
            }

            Uint8 r = round(r_sum / k_sum);
	    Uint8 g = round(g_sum / k_sum);
            Uint8 b = round(b_sum / k_sum);
            pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }

    free(temp);
    free(kernel);

    SDL_UnlockSurface(surface);
}
