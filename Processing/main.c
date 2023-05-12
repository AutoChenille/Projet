#include "hough_transform.h"
#include "grid_detection.h"
#include "contour_manager.h"
#include "img_upgrade.h"
#include "../Preprocessing/pretreatment.h"
#include <SDL2/SDL_surface.h>

double noise_level(SDL_Surface* surface)
{
    int width = surface->w;
    int height = surface->h;

    double count = 0.0;
    for (int i = 1; i < width - 1; i++)
    {
        for (int j = 1; j < height - 1; j++)
	{
            double medium = 0.0;

            // Calculate the average of the neighboring pixels
            for (int ki = -1; ki <= 1; ki++)
	    {
                for (int kj = -1; kj <= 1; kj++)
		{
                    Uint32 neighborPixel = ((Uint32*)surface->pixels)[(j + kj) * width + (i + ki)];
                    Uint8 r, g, b;
                    SDL_GetRGB(neighborPixel, surface->format, &r, &g, &b);
                    medium += r;
                }
            }
            medium /= 9;

            // Get the value of the current pixel
            Uint32 pixel = ((Uint32*)surface->pixels)[j * width + i];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            double val = 1 - (r / medium);
            if (val < 0)
                val *= -1;
            if (val > 0.5)
                count++;
        }
    }

    return count;
}

void adaptive_threshold_with_noise(SDL_Surface* surface, const double t)
{
    int width = surface->w;
    int height = surface->h;

    int s2 = fmax(width, height) / 16;
    unsigned long *thresh = calloc(width * height, sizeof(unsigned long));
    long sum = 0;
    unsigned int count = 0;
    int x1, y1, x2, y2;

    for (int y = 0; y < height; y++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);
        sum += r;
        thresh[y] = sum;
    }

    for (int i = 1; i < width; i++) {
        sum = 0;
        for (int j = 0; j < height; j++) {
            Uint32 pixel = ((Uint32*)surface->pixels)[i * width + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            sum += r;
            thresh[i * height + j] = thresh[(i - 1) * height + j] + sum;
        }
    }

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            x1 = fmax(i - s2, 1);
            x2 = fmin(i + s2, width - 1);
            y1 = fmax(j - s2, 1);
            y2 = fmin(j + s2, height - 1);
            count = (x2 - x1) * (y2 - y1);
            sum = thresh[x2 * height + y2] - thresh[x2 * height + (y1 - 1)] - thresh[(x1 - 1) * height + y2]
	      + thresh[(x1 - 1) * height + (y1 - 1)];

            Uint32 pixel = ((Uint32*)surface->pixels)[i * width + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            if (r * count < sum * (1.0 - t))
                ((Uint32*)surface->pixels)[i * width + j] = SDL_MapRGB(surface->format, 0, 0, 0);
            else
                ((Uint32*)surface->pixels)[i * width + j] = SDL_MapRGB(surface->format, 255, 255, 255);
        }
    }

    free(thresh);
}

/// @brief Main function to test the detection of sudoku.
SDL_Surface** ProcessImage(char path[], int NB_CELLS)
{
    // INIT - GET IMAGE
    // ====================================================
    // Creates a new surface from the image in parameter.
    SDL_Surface* surf_img = load_image(path);
    SDL_Surface* cut_img = load_image(path);
    cut_img = SDL_ConvertSurfaceFormat(cut_img, SDL_PIXELFORMAT_RGBA32, 0);
    // Checks if there is any error with the image.
    if (surf_img == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // Converts surface to the right format.
    SDL_Surface* surf_wait = SDL_ConvertSurfaceFormat(surf_img, SDL_PIXELFORMAT_RGBA32, 0);
    // Frees initial surface.
    SDL_FreeSurface(surf_img);
    // ====================================================


    // PREPROCESSING IMAGE BY OLEG
    // ==================================================== 
    surf_wait = SDL_ConvertSurfaceFormat(surf_wait, SDL_PIXELFORMAT_RGB888, 0);
    cut_img = SDL_ConvertSurfaceFormat(cut_img, SDL_PIXELFORMAT_RGB888, 0);

    //-----CUT_IMG-----//
	 
    //Grayscale Filter
    grayscale(cut_img);

    //Invert Filter
    cut_img = invert(cut_img);


    //-----SURF_WAIT-----//
    
    //Enhance Contrast
    enhance_contrast(surf_wait);
    IMG_SavePNG(surf_wait, "res/contrast.png");
    
    //Grayscale Filter
    grayscale(surf_wait);
    IMG_SavePNG(surf_wait, "res/grayscale.png");
    
    //Gaussian Blur
    gaussian_blur(surf_wait, 1, 0.5);
    IMG_SavePNG(surf_wait, "res/gaussian_blur.png");

    //Adaptive Threshold Filter
    double noise = noise_level(surf_wait);
    if (noise > 300)
      noise = 0.5;
    else
      noise = 0.15;
    adaptive_threshold_with_noise(surf_wait, noise);
    IMG_SavePNG(surf_wait, "res/adaptive_threshold.png");

    /*
    //Threshold
    threshold(surf_wait, 100);
    IMG_SavePNG(surf_wait, "res/thresh.png");
    */

    /*
    //Otsu Adaptative Filter
    printf("Otsu\n");
    otsu_adaptive_threshold(surf_wait);
    IMG_SavePNG(surf_wait, "res/otsu.png");
    */
    
    //Canny Edge Detection
    canny(surf_wait);
    IMG_SavePNG(surf_wait, "res/canny.png");
    
    //Dilate Filter
    surf_wait = dilate(surf_wait, 10);
    IMG_SavePNG(surf_wait, "res/dilate.png");
    // ====================================================
    

    // UPGRADE IMAGE - REMOVE PARASITES
    // ====================================================
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, surf_wait->w, surf_wait->h, 32, SDL_PIXELFORMAT_RGBA32);
    upgrade_exploitation(surf_wait, &surf);
    // ====================================================
    
    // CONTOURS - EDGES DETECTION
    // ====================================================
    double corners_x[4];
    double corners_y[4];
    // Gets the coordinates of the four corners of sudoku.
    get_max_points_rect(surf, corners_x, corners_y);
    double corners[4][2] = {{corners_x[0], corners_y[0]},
			    {corners_x[1], corners_y[1]},
			    {corners_x[2], corners_y[2]},
			    {corners_x[3], corners_y[3]}};
    // ====================================================

    
    // ROTATION AND NEW SQUARED SURFACE WITH ONLY SUDOKU
    // ONLY FOR ORIENTED SUDOKU
    // ====================================================
    surf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);

    surf = perspective_transform(surf, corners);
    IMG_SavePNG(surf, "res/perspective.png");

    surf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    IMG_SavePNG(surf, "res/perspectiveconverted.png");

    cut_img = perspective_transform(cut_img, corners);
    cut_img = threshold(cut_img, 150);
    IMG_SavePNG(cut_img, "res/cut_img.png");

    cut_img = SDL_ConvertSurfaceFormat(cut_img, SDL_PIXELFORMAT_RGBA32, 0);
    // ====================================================


    // HOUGH TRANSFORM - LINES DETECTION
    // ====================================================
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();
    // Performs Hough Transform Algorithm.
    hough_transform(surf, &list_theta, &list_rho, 2.1);
    // ====================================================


    // MAXIMUM DETECTION - MATRIX PERSPECTIVE TRANSFORM - CELLS EXTRACTION
    // ====================================================
    SDL_Surface** ocr_eleven = calloc(NB_CELLS * NB_CELLS, sizeof(SDL_Surface*));
    grid_detection(list_rho, list_theta, surf, NB_CELLS, ocr_eleven);
    // ====================================================

    // Frees memory.
    SDL_FreeSurface(surf);
    list_destroy(list_rho);
    list_destroy(list_theta);

    /*
    for (int i = 0; i < NB_CELLS * NB_CELLS; i++)
    {
        // printf("%p\n", ocr_eleven[i]);
        SDL_FreeSurface(ocr_eleven[i]);
    }

    free(ocr_eleven);
    */

    // End.
    return ocr_eleven;
}