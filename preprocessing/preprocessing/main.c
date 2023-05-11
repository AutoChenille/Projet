#include "pretreatment.h"

double magnitude(double gx, double gy)
{
    return sqrt(gx * gx + gy * gy);
}

double angle(double gx, double gy)
{
    return atan2(gy, gx);
}

Uint8 clip(int value)
{
    return (value < 0) ? 0 : (value > 255) ? 255 : (Uint8)value;
}

void canny_filter(SDL_Surface *surface, int blur_radius, double blur_sigma, int lower_threshold, int upper_threshold) {
    if (surface == NULL) {
        printf("Invalid surface.\n");
        return;
    }
    
    SDL_LockSurface(surface);

    // Apply Gaussian blur
    gaussian_blur(surface, blur_radius, blur_sigma);

    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;

    // Gradient intensity calculation using Sobel operator
    double *mag = (double *)calloc(width * height, sizeof(double));
    double *ang = (double *)calloc(width * height, sizeof(double));

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            Uint8 px[3][3][3];
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    Uint32 pixel = pixels[(y + i) * width + (x + j)];
                    SDL_GetRGB(pixel, surface->format, &px[i + 1][j + 1][0], &px[i + 1][j + 1][1], &px[i + 1][j + 1][2]);
                }
            }

            int gx = (-1 * px[0][0][0] - 2 * px[1][0][0] - 1 * px[2][0][0]) +
                      (1 * px[0][2][0] + 2 * px[1][2][0] + 1 * px[2][2][0]);
            int gy = (-1 * px[0][0][0] - 2 * px[0][1][0] - 1 * px[0][2][0]) +
                      (1 * px[2][0][0] + 2 * px[2][1][0] + 1 * px[2][2][0]);

            mag[y * width + x] = magnitude(gx, gy);
            ang[y * width + x] = angle(gx, gy);
        }
    }

    // Non-maximum suppression
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            double a = ang[y * width + x];
            double m = mag[y * width + x];
            double neighbour1, neighbour2;

                        if ((a > -22.5 && a <= 22.5) || (a > 157.5 || a <= -157.5)) {
                neighbour1 = mag[y * width + (x - 1)];
                neighbour2 = mag[y * width + (x + 1)];
            } else if ((a > 22.5 && a <= 67.5) || (a > -157.5 && a <= -112.5)) {
                neighbour1 = mag[(y - 1) * width + (x + 1)];
                neighbour2 = mag[(y + 1) * width + (x - 1)];
            } else if ((a > 67.5 && a <= 112.5) || (a > -112.5 && a <= -67.5)) {
                neighbour1 = mag[(y - 1) * width + x];
                neighbour2 = mag[(y + 1) * width + x];
            } else {
                neighbour1 = mag[(y - 1) * width + (x - 1)];
                neighbour2 = mag[(y + 1) * width + (x + 1)];
            }

            if (m > neighbour1 && m > neighbour2) {
                mag[y * width + x] = m;
            } else {
                mag[y * width + x] = 0;
            }
        }
    }

    // Double thresholding and edge tracking by hysteresis
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double m = mag[y * width + x];
            Uint8 value;
            if (m >= upper_threshold) {
                value = 255;
            } else if (m >= lower_threshold) {
                value = 127;
            } else {
                value = 0;
            }
            pixels[y * width + x] = SDL_MapRGB(surface->format, value, value, value);
        }
    }

    free(mag);
    free(ang);

    SDL_UnlockSurface(surface);
}

// Function to apply box blur to a pixel
Uint32 box_blur(SDL_Surface* surface, int x, int y, int size) {
    int total_r = 0, total_g = 0, total_b = 0, total_a = 0;
    int count = 0;

    SDL_PixelFormat* format = surface->format;

    for (int i = -size; i <= size; i++) {
        for (int j = -size; j <= size; j++) {
            int new_x = x + i;
            int new_y = y + j;

            if (new_x >= 0 && new_x < surface->w && new_y >= 0 && new_y < surface->h) {
                Uint32 pixel = ((Uint32*)surface->pixels)[new_y * surface->w + new_x];

                Uint8 r, g, b, a;
                SDL_GetRGBA(pixel, format, &r, &g, &b, &a);

                total_r += r;
                total_g += g;
                total_b += b;
                total_a += a;

                count++;
            }
        }
    }

    return SDL_MapRGBA(format, total_r / count, total_g / count, total_b / count, total_a / count);
}

// Function to apply box blur to an entire surface
SDL_Surface* reduce_noise(SDL_Surface* surface, int size)
{
    SDL_Surface* new_surface = SDL_ConvertSurface(surface, surface->format, 0);

    for (int y = 0; y < new_surface->h; y++) {
        for (int x = 0; x < new_surface->w; x++) {
            Uint32 new_pixel = box_blur(surface, x, y, size);
            ((Uint32*)new_surface->pixels)[y * new_surface->w + x] = new_pixel;
        }
    }

    return new_surface;
}


int main(int argc, char** argv)
{
    if (argc > 3)
    errx(EXIT_FAILURE, "Usage: image-file");

    // Initialise SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
      { 
        printf("Error initializing SDL2: %s\n", SDL_GetError());
        return 1;
      }

  char *filepath = argv[1];
  SDL_Surface *surface = IMG_Load(filepath);
  
  //int angle = str_to_int(argv[2]);
  /*
  int points[4][2] = {{628, 179},
		      {1366, 695},
		      {848, 1435},
		      {110, 917}};
  
  double angle = detect_angle(points);

  surface = rotate(surface, angle);
  IMG_SavePNG(surface, "rotated.png");
  */

  //Converting the surface to a new format to ensure that it is compatible with the display hardware
  //and the current rendering context, and that it can be efficiently drawn to the screen.
  surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);

  //Perspective Correction
  /*
  double corners[4][2] = {{628, 179},
  {1366, 695},
  {848, 1435},
  {110, 917}};
  surface = perspective_correction(surface, corners);
  IMG_SavePNG(surface, "perspective.png");
  */

  //Enhance Contrast
  enhance_contrast(surface);
  IMG_SavePNG(surface, "res/contrast.png");
  
  //Grayscale Filter
  grayscale(surface);
  IMG_SavePNG(surface, "res/grayscale.png");

  //Gaussian Blur
  gaussian_blur(surface, 3, 1.5);
  IMG_SavePNG(surface, "res/gaussian_blur.png");

  //Threshold Filter
  surface = threshold(surface, 110);
  IMG_SavePNG(surface, "res/threshold.png");
  
  //Otsu Adaptative Filter
  otsu_adaptive_threshold(surface);
  IMG_SavePNG(surface, "res/otsu.png");

  //Threshold Filter
  surface = threshold(surface, 100);
  IMG_SavePNG(surface, "res/threshold.png");

  /*
  //Sauvola Filter
  sauvola(surface, 15, 0.3);
  IMG_SavePNG(surface, "res/sauvola.png");
  */
  
  //Invert Filter
  surface = invert(surface);
  IMG_SavePNG(surface, "res/invert.png");

  //Dilate Filter
  surface = dilate(surface, 3);
  IMG_SavePNG(surface, "res/dilate.png");

  /*
  //Reduce Noise
  surface = reduce_noise(surface, 3);
  IMG_SavePNG(surface, "res/reduce_noise.png");
  
  //Sauvola Filter
  sauvola_filter(surface, 15, 0.3);
  IMG_SavePNG(surface, "sauvola.png");

  //Median Filter
  median_filter(surface, 5);
  IMG_SavePNG(surface, "median.png");

  //Adaptative Threshold Filter
  adaptive_threshold(surface, 31, 2);
  IMG_SavePNG(surface, "res/adaptive_threshold.png");

  //Otsu Adaptative Filter
  otsu_adaptive_threshold(surface);
  IMG_SavePNG(surface, "otsu.png");

  //Sobel Filter
  sobel_filter(surface);
  IMG_SavePNG(surface, "sobel.png");

  //Grayscale FIlter
  grayscale(surface);
  IMG_SavePNG(surface, "grayscale.png");
  
  //Canny Filter
  canny_filter(surface, 3, 1.5, 25, 75);
  IMG_SavePNG(surface, "canny.png");
  */
  SDL_FreeSurface(surface);
  SDL_Quit();
  
  return 0;
}
