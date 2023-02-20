#include "hough_transform.h"

// Number of theta for each pixel.
const double MAX_THETA = 360;

/// @brief Main function to perform the algorithm of Hough.
///
/// @param surf Surface from the initial image.
/// @param list_theta Destination list for lines detected.
/// @param list_rho Destination list for lines detected.
/// @return nothing but fills list_theta and list_rho with the coordinates of all lines detected.
void hough_transform(SDL_Surface* surf, struct list** list_theta, struct list** list_rho, const double THRESHOLD_PICK_LINES)
{
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

    int size_theta_array = MAX_THETA;
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

    // Gets threshold to filter lines.
    int max_acc = max_array(accumulator, size_acc);
    int threshold = (int) ((double) max_acc / THRESHOLD_PICK_LINES);

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

                // Saves values of rho and theta.
                *list_rho = list_insert_head(*list_rho, line_rho);
                *list_theta = list_insert_head(*list_theta, line_theta);
            }
        }
    }

    // Frees memory.
    free(accumulator);
}