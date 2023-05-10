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
    //Enhance Contrast
    enhance_contrast(surf_wait);
    IMG_SavePNG(surf_wait, "res/contrast.png");
    
    //Grayscale Filter
    grayscale(surf_wait);
    IMG_SavePNG(surf_wait, "res/grayscale.png");
    
    //Gaussian Blur
    gaussian_blur(surf_wait, 3, 1.5);
    IMG_SavePNG(surf_wait, "res/gaussian_blur.png");

    //Threshold Filter
    surf_wait = threshold(surf_wait, 90);
    IMG_SavePNG(surf_wait, "res/threshold.png");

    
    //Otsu Adaptative Filter
    otsu_adaptive_threshold(surf_wait);
    IMG_SavePNG(surf_wait, "res/otsu.png");
    
    /*
    //Sauvola Filter
    sauvola(surface, 15, 0.3);
    IMG_SavePNG(surface, "res/sauvola.png");
    */
    
    //Invert Filter
    surf_wait = invert(surf_wait);
    IMG_SavePNG(surf_wait, "res/invert.png");

    //Dilate Filter
    surf_wait = dilate(surf_wait, 2);
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
    
    //Grayscale FIlter
    grayscale(surface);
    IMG_SavePNG(surface, "grayscale.png");
    
    //Canny Filter
    canny_filter(surface, 3, 1.5, 25, 75);
    IMG_SavePNG(surface, "canny.png");
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
    printf("%s\n", "coucou-1");
    double corners[4][2] = {{corners_x[0], corners_y[0]},
			    {corners_x[1], corners_y[1]},
			    {corners_x[2], corners_y[2]},
			    {corners_x[3], corners_y[3]}};
    printf("%s\n", "coucou0");
    for (int i = 0; i<4; i++)
      printf("x = %f / y = %f\n", corners[i][0], corners[i][1]);
    // ====================================================

    // ROTATION AND NEW SQUARED SURFACE WITH ONLY SUDOKU
    // ONLY FOR ORIENTED SUDOKU
    // ====================================================
    printf("%s\n", "coucou1");
    surf = cropSurface(surf, corners[0][0], corners[0][1], corners[2][0], corners[2][1]);
    IMG_SavePNG(surf, "res/crop.png");

    //double angle = detect_angle((int(*)[2])corners);
    double angle = calculateSudokuRotation((int(*)[2])corners);
    printf("angle = %f\n", angle);
    printf("%s\n", "coucou2");
    //surf = rotate(surf, angle);
    //IMG_SavePNG(surf, "res/rotated.png");
    printf("%s\n", "coucou3");

    //surf = perspective_correction(surf, corners);
    //IMG_SavePNG(surf, "perspective.png");

    // ====================================================


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
    grid_detection(list_rho, list_theta, surf);
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
