#include "hough_transform.h"
#include "grid_detection.h"
#include "contour_manager.h"
#include "img_upgrade.h"
#include "../preprocessing/pretreatment.h"


/// @brief Main function to test the detection of sudoku.
int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-path");


    // INIT - GET IMAGE
    // ====================================================
    // Creates a new surface from the image in parameter.
    SDL_Surface* surf_img = load_image(argv[1]);
    SDL_Surface* cut_img = load_image(argv[1]);
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

    //Median Filter
    //median_filter(surf_wait, 5);
    //IMG_SavePNG(surf_wait, "res/median.png");

    //Threshold Filter
    surf_wait = threshold(surf_wait, 150);
    IMG_SavePNG(surf_wait, "res/threshold.png");    
    
    //Otsu Adaptative Filter
    otsu_adaptive_threshold(surf_wait);
    IMG_SavePNG(surf_wait, "res/otsu.png");
    
    //Canny Edge Detection
    canny(surf_wait);
    IMG_SavePNG(surf_wait, "res/canny.png");

    /*
    //Invert Filter
    surf_wait = invert(surf_wait);
    IMG_SavePNG(surf_wait, "res/invert.png");
    */
    
    /*
    //Sauvola Filter
    sauvola(surf_wait, 15, 0.5);
    IMG_SavePNG(surf_wait, "res/sauvola.png");
    */
    
    //Dilate Filter
    surf_wait = dilate(surf_wait, 10);
    IMG_SavePNG(surf_wait, "res/dilate.png");
    
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
    */
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
    for (int i = 0; i<4; i++)
      printf("x = %f / y = %f\n", corners[i][0], corners[i][1]);
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

    printf("%s\n", "coucou3");

    // HOUGH TRANSFORM - LINES DETECTION
    // ====================================================
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();
    // Performs Hough Transform Algorithm.
    hough_transform(surf, &list_theta, &list_rho, 2.1);
    printf("%s\n", "coucou4");
    // ====================================================


    // MAXIMUM DETECTION - MATRIX PERSPECTIVE TRANSFORM - CELLS EXTRACTION
    // ====================================================
    grid_detection(list_rho, list_theta, cut_img);
    printf("%s\n", "coucou5");
    // ====================================================


    // Frees memory.
    SDL_FreeSurface(surf);
    SDL_FreeSurface(surf_wait);
    list_destroy(list_rho);
    list_destroy(list_theta);

    // End.
    return EXIT_SUCCESS;
}
