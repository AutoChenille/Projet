// Libraries.
// ========================
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// ========================

// Files.
// ========================
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// ========================

// Main function to perform the algorithm of Hough.
//
// path: path of an image.
// return: nothing.
void hough_transform(char path[], int debug)
{
    // Creates a new surface from an image.
    SDL_Surface* surf = load_image(path);

    // Gets the length and the width of the surface.
    int width = surf->w;
    int height = surf->h;

    // Initializes a new matrix of bits (0 or 1).
    // And gets array of 0 and 1 from a surface.
    int pixel_array[width * height];

    // Gets the array of pixels from the surface.
    get_array_of_pixels(surf, pixel_array);

    // Rho parameters.
    // ==============================
    double width_db = (double) width;
    double height_db = (double) height;
    double surf_diag = sqrt(width_db * width_db + height_db * height_db);
    double rho_min = -surf_diag;
    double rho_max = surf_diag;

    int size_rho_array = (int) fabs(rho_min) + (int) fabs(rho_max);
    double rho_array[size_rho_array];
    make_double_array(size_rho_array, rho_min, rho_max, rho_array);
    // ==============================

    // Theta parameters.
    // =================
    int theta_min = -90;
    int theta_max = 90;

    int size_theta_array = 800;
    double arr_theta[size_theta_array];
    make_double_array(size_theta_array, theta_min, theta_max, arr_theta);
    // =================

    // Converts theta array in radians.
    for (int i = 0; i < size_theta_array; i++)
    {
        arr_theta[i] = deg_to_rad(arr_theta[i]);
    }

    // Accumulator initialization and parameters.
    // Rows are for Rhos and columns are for Thetas.
    // =========================================================
    int number_theta_acc = size_theta_array;
    int number_rho_acc = size_rho_array;
    int size_acc = number_rho_acc * number_theta_acc;

    int *accumulator = calloc(size_acc, sizeof(int));
    if (accumulator == NULL)
        err(EXIT_FAILURE, "%s", "Accumulator not initialized");
    // =========================================================

    // Set all votes in accumulator.
    // For each pixel.
    for (int x = 0; x < (int) width; x++)
    {
        for (int y = 0; y < (int) height; y++)
        {
            // If pixel is white.
            if (pixel_array[x + width * y] == 1)
            {
                // For each values of theta.
                for (int i = 0; i < size_theta_array; i++)
                {
                    // Computes the rho relative to theta.
                    double rho_pixel = x * cos(arr_theta[i]) + y * sin(arr_theta[i]);

                    // Gets rho index in accumulator array.
                    // Rho is in [-diag,diag].
                    // We need to add rho_max to avoid negative index.
                    int rho_index = (int) rho_pixel + (int) rho_max;

                    // Adds one vote in accumulator at coordinates (theta, rho).
                    accumulator[i + (int) number_theta_acc * rho_index]++;
                }
            }
        }
    }

    // DEBUG.
    // ===============================================================================================
    if (debug)
    {
        // Gets surface from accumulator.
        SDL_Surface* surf_acc = array_to_surface(number_theta_acc, size_rho_array, accumulator);

        // Save image of accumulator in PNG format.
        IMG_SavePNG(surf_acc, "hough_diagram.png");

        // Frees memory.
        SDL_FreeSurface(surf_acc);
    }
    // ===============================================================================================

    // Gets threshold to filter lines.
    int max_acc = max_array(accumulator, size_acc);
    int threshold = (int) ((double) max_acc / 2);

    // Initializes lists to store rho and theta.
    struct list* list_rho = list_new();
    struct list* list_theta = list_new();

    // Gets threshold in degrees to detect horizontal and vertical lines.
    // We suppose sudoku is the good position to solve it.
    int theta_threshold = 8;

    // Gets maximums in accumulator.
    for (int rho = 0; rho < number_rho_acc; rho++)
    {
        for (int theta = 0; theta < number_theta_acc; theta++)
        {
            // If value in accumulator is better than limit.
            if (accumulator[theta + number_theta_acc * rho] > threshold)
            {
                // Get coordinates of line.
                double line_rho = rho_array[rho];
                double line_theta = arr_theta[theta];

                // Line horizontal ? Checks in interval:
                // [0 - threshold, 0 + threshold].
                int horizontal_line = line_theta > deg_to_rad(-theta_threshold)
                        && line_theta < deg_to_rad(theta_threshold);

                // Line vertical ? Checks in interval:
                // [-90 - threshold, -90 + threshold] U [90 - threshold, 90 + threshold].
                int vertical_line =
                        (line_theta > deg_to_rad(90 - theta_threshold)
                        && line_theta < deg_to_rad(90 + theta_threshold)) ||
                        ((line_theta > deg_to_rad(-90 - theta_threshold))
                        && (line_theta < deg_to_rad(-90 + theta_threshold)));

                // Keeps the line if it is horizontal or vertical. The other are deleted.
                if (horizontal_line || vertical_line)
                {
                    // Saves values of rho and theta.
                    list_rho = list_insert_head(list_rho, line_rho);
                    list_theta = list_insert_head(list_theta, line_theta);
                }
            }
        }
    }

    // DEBUG.
    // =============================================================
    if (debug)
    {
        // Opens window with lines drawn on it and waits...
        draw_lines_on_window(list_rho, list_theta, path, surf_diag);
    }
    // =============================================================


    // Separates horizontal lines and vertical lines.

    // Gets points of intersection.





    // Frees memory.
    free(accumulator);
    SDL_FreeSurface(surf);
    list_destroy(list_rho);
    list_destroy(list_theta);
}

// Test function.
int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    hough_transform(argv[1], 1);

    return EXIT_SUCCESS;
}