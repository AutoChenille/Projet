#include "hough_transform.h"

// Test function.
int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // Initializes lists to store rho and theta.
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();

    // Creates a new surface from the image in parameter.
    SDL_Surface* surf = load_image(argv[1]);

    // Performs Hough transform to get lists of thetas and rhos of all lines.
    hough_transform(surf, 1, list_theta, list_rho);

    return EXIT_SUCCESS;
}