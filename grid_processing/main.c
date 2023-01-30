#include "hough_transform.h"
#include "grid_detection.h"

// Test function.
int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file-path");

    // Initializes lists to store rho and theta.
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();

    // Creates a new surface from the image in parameter.
    SDL_Surface* surf_img = load_image(argv[1]);
    if (surf_img == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Converts surface to the right format.
    SDL_Surface* surf = SDL_ConvertSurfaceFormat(surf_img, SDL_PIXELFORMAT_RGBA32, 0);

    // Frees initial surface.
    SDL_FreeSurface(surf_img);

    // Performs Hough Transform to get lists of thetas and rhos of all lines.
    // Now we have all equations of lines detected in polar coordinates.
    hough_transform(surf, 0, &list_theta, &list_rho);

    // Rotation of image  needed. Now we have only vertical and horizontal lines fpr sudoku.
    // ...
    // Waiting for someone...
    // ...

    double w = surf->w;
    double h = surf->h;
    grid_detection(0, list_rho, list_theta, sqrt(w * w + h * h), surf);

    // Frees memory.
    SDL_FreeSurface(surf);

    return EXIT_SUCCESS;
}