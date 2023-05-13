#include "hough_transform.h"
#include "grid_detection.h"
#include "contour_manager.h"
#include "img_upgrade.h"
#include "../Preprocessing/pretreatment.h"

/// @brief Main function to test the detection of sudoku.
SDL_Surface** ProcessImage(char *path, int NB_CELLS)
{
    // INIT - GET IMAGE
    // ====================================================
    // Creates a new surface from the image in parameter.
    SDL_Surface* surf_img = load_image(path);
    SDL_Surface* cut_img = load_image(path);
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

    //-----SURF_WAIT-----//
    
    //Enhance Contrast
    enhance_contrast(surf_wait);
    
    //Grayscale Filter
    grayscale(surf_wait);
    
    //Gaussian Blur
    gaussian_blur(surf_wait, 1, 0.5);

    //Adaptive Threshold Filter
    double noise = noise_level(surf_wait);
    printf("noise = %f\n", noise);
    if (noise > 15)
    {
      printf("adaptive\n");
      noise = 0.5;
      adaptive_threshold(surf_wait, noise);
    }
    else
    {
      printf("non-adaptive\n");
      noise = 0.2;
      adaptive_threshold(surf_wait, noise);
      //surf_wait = threshold(surf_wait, 150);
    }
    SDL_BlitSurface(surf_wait, NULL, cut_img, NULL);
    IMG_SavePNG(surf_wait, "Processing/res/adaptive_threshold.png");

    //Invert Filter
    cut_img = invert(cut_img);

    //Canny Edge Detection
    canny(surf_wait);
    IMG_SavePNG(surf_wait, "Processing/res/canny.png");

    //Dilate Filter
    dilate(surf_wait);
    IMG_SavePNG(surf_wait, "Processing/res/dilate.png");
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
    IMG_SavePNG(surf, "Processing/res/perspective.png");

    surf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);

    cut_img = perspective_transform(cut_img, corners);
    IMG_SavePNG(cut_img, "Processing/res/cut_img.png");
    IMG_SavePNG(cut_img, "pre.png");

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
    grid_detection(list_rho, list_theta, cut_img, NB_CELLS, ocr_eleven);
    // ====================================================

    // Frees memory.
    SDL_FreeSurface(surf);
    SDL_FreeSurface(surf_wait);
    SDL_FreeSurface(cut_img);
    list_destroy(list_rho);
    list_destroy(list_theta);

    // End.
    return ocr_eleven;
}
