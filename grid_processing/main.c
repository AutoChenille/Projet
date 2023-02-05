#include "hough_transform.h"
#include "grid_detection.h"
#include "img_upgrade.h"
#include "contour_manager.h"

// Test function.
int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-path");


    // INIT - GET IMAGE
    // ====================================================
    // Creates a new surface from the image in parameter.
    SDL_Surface* surf_img = load_image(argv[1]);
    if (surf_img == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Converts surface to the right format.
    SDL_Surface* surf = SDL_ConvertSurfaceFormat(surf_img, SDL_PIXELFORMAT_RGBA32, 0);

    // Frees initial surface.
    SDL_FreeSurface(surf_img);
    // ====================================================


    // PREPROCESSING IMAGE
    // ====================================================



    // ====================================================


    // CONTOURS - EDGES DETECTION
    // ====================================================
    struct list* corners_x = list_new();
    struct list* corners_y = list_new();
    get_max_points_rect(surf, &corners_x, &corners_y);
    // ====================================================


    // ROTATION AND NEW SQUARED SURFACE WITH ONLY SUDOKU
    // ====================================================



    // ====================================================


    // HOUGH TRANSFORM - LINES DETECTION
    // ====================================================
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();
    hough_transform(0, surf, &list_theta, &list_rho);
    // ====================================================


    // MAXIMUM DETECTION - MATRIX PERSPECTIVE TRANSFORM - CELLS EXTRACTION
    // ====================================================
    double w = surf->w;
    double h = surf->h;
    grid_detection(0, list_rho, list_theta, sqrt(w * w + h * h), surf);
    // ====================================================


    // Frees memory.
    SDL_FreeSurface(surf);
    list_destroy(list_rho);
    list_destroy(list_theta);
    list_destroy(corners_x);
    list_destroy(corners_y);

    // End.
    return EXIT_SUCCESS;
}