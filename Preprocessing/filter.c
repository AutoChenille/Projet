#include "pretreatment.h"

// Enchance surface's contrasts
void enhance_contrast(SDL_Surface *surface)
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
    Uint8 min_r = 255, min_g = 255, min_b = 255;
    Uint8 max_r = 0, max_g = 0, max_b = 0;

    // Find the minimum and maximum pixel values
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
	{
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
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
	{
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


// Same function from the TP06.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format)
{
  /* Takes a pixel and a surface's format and convert the pixel into grayscale pixel */
  Uint8 r, g, b;

  SDL_GetRGB(pixel_color, format, &r, &g, &b);

  Uint8 average = (0.3 * r) +  (0.59 * g) + (0.11 * b);

  r = g = b = average;

  Uint32 color = SDL_MapRGB(format, r, g, b);

  return color;
}

// Same function from the TP06.
void grayscale(SDL_Surface *surface)
{
  /* Takes a surface as a parameter and grayscale all the pixels of the surface */
  Uint32 *pixels = surface->pixels;
  int len = surface->w * surface->h;
  SDL_PixelFormat *format = surface->format;
  int i = 0;

  SDL_LockSurface(surface);

  while (i < len)
    {
      pixels[i] = pixel_to_grayscale(pixels[i], format);
      i++;
    }

  SDL_UnlockSurface(surface);
}

const double PI = 3.14159265358979323846;

double gaussian(double x, double sigma)
{
    return (1.0 / (sigma * sqrt(2.0 * PI))) * exp(-(x * x) / (2.0 * sigma * sigma));
}

// Apply the gaussian blur filter to a grayscales surface
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
    for (int i = 0; i < 2 * radius + 1; i++)
        kernel[i] = gaussian(i - radius, sigma);

    Uint32 *temp = (Uint32 *) malloc(width * height * sizeof(Uint32));
    memcpy(temp, pixels, width * height * sizeof(Uint32));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double r_sum = 0, g_sum = 0, b_sum = 0, k_sum = 0;
            
            for (int i = -radius; i <= radius; i++)
            {
                int xi = x + i;
                if (xi >= 0 && xi < width) 
                {
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

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double r_sum = 0, g_sum = 0, b_sum = 0, k_sum = 0;
            
            for (int i = -radius; i <= radius; i++)
            {
                int yi = y + i;
                if (yi >= 0 && yi < height)
                {
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

// Calculates the noise level of a surface
double noise_level(SDL_Surface* surface)
{
    int width = surface->w;
    int height = surface->h;

    int count = 0;
    
    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
	    {
            double average = 0.0;

            // Calculate the average of the neighboring pixels
            for (int k = -1; k <= 1; k++)
	        {
                for (int l = -1; l <= 1; l++)
		        {
                    Uint32 neighbor = ((Uint32*)surface->pixels)[(j + l) * width + (i + k)];
                    Uint8 r, g, b;
                    
                    SDL_GetRGB(neighbor, surface->format, &r, &g, &b);
                    average += r;
                }
            }
            average /= 9;

            // Get the value of the current pixel
            Uint32 pixel = ((Uint32*)surface->pixels)[j * width + i];
            Uint8 r, g, b;
            
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            
            double val = 1 - (r / average);
            
            if (val < 0)
                val *= -1;
            if (val > 0.5)
                count++;
        }
    }

    return (double)count;
}

// Applies the adaptive threshold algorithm to a surface with a given threshold parameter
void adaptive_threshold(SDL_Surface* surface, double threshold)
{
    int width = surface->w;
    int height = surface->h;

    int parameter = fmax(width, height) / 16;
    unsigned long *thresh = calloc(width * height, sizeof(unsigned long));
    long sum = 0;
    unsigned int count = 0;
    int xmax, ymax, xmin, ymin;

    for (int y = 0; y < height; y++)
    {
        Uint32 pixel = ((Uint32*)surface->pixels)[y];
        Uint8 r, g, b;
        
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);
        sum += r;
        thresh[y] = sum;
    }

    for (int i = 1; i < width; i++)
    {
        sum = 0;
        for (int j = 0; j < height; j++)
	{
            Uint32 pixel = ((Uint32*)surface->pixels)[i * width + j];
            Uint8 r, g, b;
            
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            sum += r;
            thresh[i * height + j] = thresh[(i - 1) * height + j] + sum;
        }
    }

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
	{
            xmax = fmax(i - parameter, 1);
            xmin = fmin(i + parameter, width - 1);
            ymax = fmax(j - parameter, 1);
            ymin = fmin(j + parameter, height - 1);
            count = (xmin - xmax) * (ymin - ymax);
            sum = thresh[xmin * height + ymin] - thresh[xmin * height + (ymax - 1)] - thresh[(xmax - 1) * height + ymin]
	        + thresh[(xmax - 1) * height + (ymax - 1)];

            Uint32 pixel = ((Uint32*)surface->pixels)[j * width + i];
            Uint8 r, g, b;
            
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r * count < sum * (1.0 - threshold))
                ((Uint32*)surface->pixels)[j * width + i] = SDL_MapRGB(surface->format, 0, 0, 0);
            else
                ((Uint32*)surface->pixels)[j * width + i] = SDL_MapRGB(surface->format, 255, 255, 255);
        }
    }

    free(thresh);
}

/*
void dilate(SDL_Surface* surface)
{
    int width = surface->w;
    int height = surface->h;

    SDL_Surface* temp = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, surface->format->format);

    SDL_BlitSurface(surface, NULL, temp, NULL); // Copy original surface to temp

    SDL_LockSurface(surface);
    SDL_LockSurface(temp);

    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32* tempPixels = (Uint32*)temp->pixels;

    for (int y = 4; y < height - 4; ++y) {
        for (int x = 4; x < width - 4; ++x) {
            Uint8 r, g, b;

            // Check the 9x9 neighborhood
            for (int j = -4; j <= 4; ++j) {
                for (int i = -4; i <= 4; ++i) {
                    Uint32 pixel = tempPixels[(y + j) * width + (x + i)];
                    SDL_GetRGB(pixel, temp->format, &r, &g, &b);

                    // If a white pixel is found, set the current pixel to white and break the loop
                    if (r == 255 && g == 255 && b == 255) {
                        pixels[y * width + x] = SDL_MapRGB(surface->format, 255, 255, 255);
                        i = 5;
                        j = 5;
                    }
                }
            }
        }
    }

    SDL_UnlockSurface(temp);
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(temp);
}
*/

// Applies the dilate filter to a surface 10x10 (OPTIMIZED)
void dilate(SDL_Surface* surface)
{
    int width = surface->w;
    int height = surface->h;

    SDL_Surface* temp = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, surface->format->format);

    SDL_BlitSurface(surface, NULL, temp, NULL); // Copy original surface to temp

    SDL_LockSurface(surface);
    SDL_LockSurface(temp);

    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32* tempPixels = (Uint32*)temp->pixels;

    // Horizontal dilation
    for (int y = 0; y < height; ++y)
    {
        for (int x = 5; x < width - 5; ++x)
        {
            Uint8 r, g, b;

            for (int i = -5; i <= 5; ++i)
            {
                Uint32 pixel = tempPixels[y * width + (x + i)];
                SDL_GetRGB(pixel, temp->format, &r, &g, &b);

                if (r == 255 && g == 255 && b == 255)
                {
                    pixels[y * width + x] = SDL_MapRGB(surface->format, 255, 255, 255);
                    break;
                }
            }
        }
    }

    SDL_UnlockSurface(temp);

    SDL_BlitSurface(surface, NULL, temp, NULL); // Copy dilated surface to temp

    // Vertical dilation
    for (int y = 5; y < height - 5; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Uint8 r, g, b;

            for (int i = -5; i <= 5; ++i)
            {
                Uint32 pixel = tempPixels[(y + i) * width + x];
                SDL_GetRGB(pixel, temp->format, &r, &g, &b);

                if (r == 255 && g == 255 && b == 255)
                {
                    pixels[y * width + x] = SDL_MapRGB(surface->format, 255, 255, 255);
                    break;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
    SDL_FreeSurface(temp);
}

// Invert the colors of a surface
SDL_Surface* invert(SDL_Surface *input)
{
    if (input == NULL)
    {
        printf("Invalid surface.\n");
        return NULL;
    }

    SDL_Surface* output = SDL_ConvertSurfaceFormat(input, input->format->format, 0);
    if (output == NULL)
    {
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

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            double dx = 0.0;
            double dy = 0.0;

            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
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

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            double dir = direction[y][x];
            dir += 180;
            dir = fmod(dir, 180);

            if ((dir >= 0 && dir < 22.5) || (dir >= 157.5 && dir < 180))
            {
                if (gradient[y][x] <= gradient[y][x - 1] || gradient[y][x] <= gradient[y][x + 1])
                    gradient[y][x] = 0;
            } 
            else if (dir >= 22.5 && dir < 67.5)
            {
                if (gradient[y][x] <= gradient[y - 1][x + 1] || gradient[y][x] <= gradient[y + 1][x - 1])
                    gradient[y][x] = 0;
            }
            else if (dir >= 67.5 && dir < 112.5)
            {
                if (gradient[y][x] <= gradient[y - 1][x] || gradient[y][x] <= gradient[y + 1][x])
                    gradient[y][x] = 0;
	        } 
            else if (dir >= 112.5 && dir < 157.5)
            {
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

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            Uint32 pixel = ((Uint32*)grayscaleImage->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, grayscaleImage->format, &r, &g, &b);

            if (r == 125) {
                Uint8 val = 0;
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
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
    for (int i = 0; i < height; i++)
    {
        gradient[i] = (double*)malloc(width * sizeof(double));
        direction[i] = (double*)malloc(width * sizeof(double));
    }

    calculateGradientAndDirection(grayscaleImage, gradient, direction);
    nonMaximumSuppression(grayscaleImage, gradient, direction);
    doubleThreshold(grayscaleImage, gradient);
    edgeTracking(grayscaleImage);

    // Free the gradient and direction matrices
    for (int i = 0; i < height; i++)
    {
        free(gradient[i]);
        free(direction[i]);
    }
    free(gradient);
    free(direction);
}

// Apply the threshold algorithm with a given threshold as a parameter
SDL_Surface* threshold(SDL_Surface *input, Uint8 threshold_value)
{
    if (input == NULL)
    {
        printf("Invalid surface.\n");
        return NULL;
    }

    SDL_Surface* output = SDL_ConvertSurfaceFormat(input, input->format->format, 0);
    if (output == NULL)
    {
        printf("Failed to create output surface: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_LockSurface(output);
    Uint32 *pixels = (Uint32 *)output->pixels;

    int width = output->w;
    int height = output->h;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y * width + x];
            SDL_GetRGB(pixel, output->format, &r, &g, &b);

            // Convert to grayscale using the average of the three color channels
            Uint8 grayscale = (r + g + b) / 3;
            
            // Thresholding
            if (grayscale > threshold_value)
            {
                pixels[y * width + x] = SDL_MapRGB(output->format, 255, 255, 255);
            } 
            else
            {
                pixels[y * width + x] = SDL_MapRGB(output->format, 0, 0, 0);
            }
        }
    }

    SDL_UnlockSurface(output);
    
    return output;
}
