#include "pretreatment.h"

#define PI 3.14159265
#define HIGH_THRESHOLD 75
#define LOW_THRESHOLD 30

// Calculate Gradient and Direction
void calculateGradientAndDirection(SDL_Surface* grayscaleImage, double** gradient, double** direction)
{
    int sobelX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int sobelY[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };

    int width = grayscaleImage->w;
    int height = grayscaleImage->h;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            double dx = 0.0;
            double dy = 0.0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    Uint32 pixel = ((Uint32*)grayscaleImage->pixels)[(y + i) * width + (x + j)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, grayscaleImage->format, &r, &g, &b);
                    dx += (double)r * sobelX[i + 1][j + 1];
                    dy += (double)r * sobelY[i + 1][j + 1];
                }
            }

            gradient[y][x] = sqrt(dx * dx + dy * dy);
            direction[y][x] = atan2(dy, dx) * 180 / PI;
        }
    }
}

// Non-Maximum Suppression
void nonMaximumSuppression(SDL_Surface* grayscaleImage, double** gradient, double** direction)
{
    int width = grayscaleImage->w;
    int height = grayscaleImage->h;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            double dir = direction[y][x];
            dir += 180;
            dir = fmod(dir, 180);

            if ((dir >= 0 && dir < 22.5) || (dir >= 157.5 && dir < 180)) {
                if (gradient[y][x] <= gradient[y][x - 1] || gradient[y][x] <= gradient[y][x + 1])
                    gradient[y][x] = 0;
            } else if (dir >= 22.5 && dir < 67.5) {
                if (gradient[y][x] <= gradient[y - 1][x + 1] || gradient[y][x] <= gradient[y + 1][x - 1])
                    gradient[y][x] = 0;
            } else if (dir >= 67.5 && dir < 112.5) {
                if (gradient[y][x] <= gradient[y - 1][x] || gradient[y][x] <= gradient[y + 1][x])
                    gradient[y][x] = 0;
	    } else if (dir >= 112.5 && dir < 157.5) {
                if (gradient[y][x] <= gradient[y - 1][x - 1] || gradient[y][x] <= gradient[y + 1][x + 1])
                    gradient[y][x] = 0;
            }
        }
    }
}

// Double Threshold
void doubleThreshold(SDL_Surface* grayscaleImage, double** gradient)
{
    int width = grayscaleImage->w;
    int height = grayscaleImage->h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 val = (gradient[y][x] > HIGH_THRESHOLD) ? 255 : (gradient[y][x] > LOW_THRESHOLD) ? 125 : 0;
            Uint32 newPixel = SDL_MapRGB(grayscaleImage->format, val, val, val);
            ((Uint32*)grayscaleImage->pixels)[y * width + x] = newPixel;
        }
    }
}

// Edge Tracking by Hysteresis
void edgeTracking(SDL_Surface* grayscaleImage)
{
    int width = grayscaleImage->w;
    int height = grayscaleImage->h;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            Uint32 pixel = ((Uint32*)grayscaleImage->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, grayscaleImage->format, &r, &g, &b);

            if (r == 125) {
                Uint8 val = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        Uint32 neighborPixel = ((Uint32*)grayscaleImage->pixels)[(y + i) * width + (x + j)];
                        Uint8 nr, ng, nb;
                        SDL_GetRGB(neighborPixel, grayscaleImage->format, &nr, &ng, &nb);
                        if (nr == 255) {
                            val = 255;
                            break;
                        }
                    }
                    if (val == 255) break;
                }

                Uint32 newPixel = SDL_MapRGB(grayscaleImage->format, val, val, val);
                ((Uint32*)grayscaleImage->pixels)[y * width + x] = newPixel;
            }
        }
    }
}

// Canny Edge Detection
void canny(SDL_Surface* grayscaleImage)
{
    int width = grayscaleImage->w;
    int height = grayscaleImage->h;

    // Initialize gradient and direction matrices
    double** gradient = (double**)malloc(height * sizeof(double*));
    double** direction = (double**)malloc(height * sizeof(double*));
    for (int i = 0; i < height; i++) {
        gradient[i] = (double*)malloc(width * sizeof(double));
        direction[i] = (double*)malloc(width * sizeof(double));
    }

    calculateGradientAndDirection(grayscaleImage, gradient, direction);
    nonMaximumSuppression(grayscaleImage, gradient, direction);
    doubleThreshold(grayscaleImage, gradient);
    edgeTracking(grayscaleImage);

    // Free the gradient and direction matrices
    for (int i = 0; i < height; i++) {
        free(gradient[i]);
        free(direction[i]);
    }
    free(gradient);
    free(direction);
}


