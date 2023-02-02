#include "hough_transform.h"
#include "grid_detection.h"
#include "img_upgrade.h"

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


    // PREPROCESSING - REMOVE PARASITES
    // ====================================================
    // Creates a new surface.
    SDL_Surface* surf_processed = SDL_CreateRGBSurfaceWithFormat(0, surf->w, surf->h, 32, SDL_PIXELFORMAT_RGBA32);

    // Upgrade image to remove big parasites zones.
    upgrade_exploitation(1, surf, &surf_processed);
    if (surf_processed == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Frees surface.
    SDL_FreeSurface(surf);
    // ====================================================


    // HOUGH TRANSFORM - LINES DETECTION
    // ====================================================
    // Initializes lists to store rho and theta.
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();

    // Performs Hough Transform to get lists of thetas and rhos of all lines.
    hough_transform(1, surf_processed, &list_theta, &list_rho);
    // ====================================================


    // EXPLOITATION - GRID & CELLS DETECTION
    // ====================================================
    double w = surf->w;
    double h = surf->h;
    grid_detection(0, list_rho, list_theta, sqrt(w * w + h * h), surf_processed);
    // ====================================================


    // Frees memory.
    SDL_FreeSurface(surf_processed);
    list_destroy(list_rho);
    list_destroy(list_theta);

    return EXIT_SUCCESS;
}