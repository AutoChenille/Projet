#include "hough_transform.h"
#include "grid_detection.h"
#include "contour_manager.h"
#include "img_upgrade.h"


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

    // UPGRADE IMAGE - REMOVE PARASITES
    // ====================================================
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, surf_wait->w, surf_wait->h, 32, SDL_PIXELFORMAT_RGBA32);
    upgrade_exploitation(surf_wait, &surf);
    // ====================================================


    // PREPROCESSING IMAGE BY OLEG
    // ====================================================
    // ...
    // Coming soon...
    // ...
    // ====================================================


    // CONTOURS - EDGES DETECTION
    // ====================================================
    double corners_x[4];
    double corners_y[4];
    // Gets the coordinates of the four corners of sudoku.
    get_max_points_rect(surf, corners_x, corners_y);

    // ====================================================

    // ROTATION AND NEW SQUARED SURFACE WITH ONLY SUDOKU
    // ONLY FOR ORIENTED SUDOKU
    // ====================================================
    // ...
    // Coming soon...
    // ..
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
    int NB_CELLS = 9;
    SDL_Surface** ocr_eleven = calloc(NB_CELLS * NB_CELLS, sizeof(SDL_Surface*));
    grid_detection(list_rho, list_theta, surf, NB_CELLS, ocr_eleven);
    // ====================================================

    // Frees memory.
    SDL_FreeSurface(surf);
    list_destroy(list_rho);
    list_destroy(list_theta);

    for (int i = 0; i < NB_CELLS * NB_CELLS; i++)
    {
        // printf("%p\n", ocr_eleven[i]);
        SDL_FreeSurface(ocr_eleven[i]);
    }

    free(ocr_eleven);

    // End.
    return EXIT_SUCCESS;
}