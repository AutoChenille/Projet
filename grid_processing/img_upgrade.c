#include "img_upgrade.h"

//
//
//
//
int fill_left(int width, int height, int* pixels)
{
    // Inits constants.
    int min_zone = height / 3;
    int max_zone = 2 * height / 3;
    double threshold = 1 / height;

    int cpt = 0;

    // Saves the last column that has been detected.
    int last_column = 0;

    // For only the third part.
    for (int i = 0; i < width / 3; i++)
    {
        // Inits the sum.
        double sum_column = 0;

        // For each pixel in the middle of the column, add its value to the sum.
        for (int j = min_zone; j < max_zone; j++)
        {
            sum_column += pixels[i + width * j];
        }

        // Computes the average of pixels on the column.
        double moy = sum_column / height;

        if (moy <= threshold)
        {
            for (int i_tmp = last_column; i_tmp < i; i_tmp++)
            {
                for (int j_tmp = 0; j_tmp < height; j_tmp++)
                {
                    pixels[i_tmp + width * j_tmp] = 0;
                    cpt++;
                }
            }

            // Save detected column.
            last_column = i;
        }
    }

    return cpt;
}

//
//
//
//
int fill_right(int width, int height, int* pixels)
{
    // Inits constants.
    int min_zone = height / 3;
    int max_zone = 2 * height / 3;
    double threshold = 1 / height;

    // Saves the last column that has been detected.
    int last_column = width - 1;

    int cpt = 0;

    // For only the third part.
    for (int i = width - 1; i > 2 * width / 3; i--)
    {
        // Inits the sum.
        double sum_column = 0;

        // For each pixel in the middle of the column, add its value to the sum.
        for (int j = min_zone; j < max_zone; j++)
        {
            sum_column += pixels[i + width * j];
        }

        // Computes the average of pixels on the column.
        double moy = sum_column / height;

        if (moy <= threshold)
        {
            for (int i_tmp = last_column; i_tmp > i; i_tmp--)
            {
                for (int j_tmp = 0; j_tmp < height; j_tmp++)
                {
                    pixels[i_tmp + width * j_tmp] = 0;
                    cpt++;
                }
            }

            // Save detected column.
            last_column = i;
        }
    }

    return cpt;
}

//
//
//
//
int fill_top(int width, int height, int* pixels)
{
    // Inits constants.
    int min_zone = width / 3;
    int max_zone = 2 * width / 3;
    double threshold = 1 / width;

    // Saves the last line that has been detected.
    int last_line = 0;

    int cpt = 0;

    // For only the third part.
    for (int i = 0; i < height / 3; i++)
    {
        // Inits the sum.
        double sum_line = 0;

        // For each pixel in the middle of the line, add its value to the sum.
        for (int j = min_zone; j < max_zone; j++)
        {
            sum_line += pixels[j + width * i];
        }

        // Computes the average of pixels on the line.
        double moy = sum_line / width;

        if (moy <= threshold)
        {
            for (int i_tmp = last_line; i_tmp < i; i_tmp++)
            {
                for (int j_tmp = 0; j_tmp < width; j_tmp++)
                {
                    pixels[j_tmp + width * i_tmp] = 0;
                    cpt++;
                }
            }

            // Save detected column.
            last_line = i;
        }
    }

    return cpt;
}

//
//
//
//
int fill_bottom(int width, int height, int* pixels)
{
    // Inits constants.
    int min_zone = width / 3;
    int max_zone = 2 * width / 3;
    double threshold = 1 / width;

    // Saves the last line that has been detected.
    int last_line = height - 1;

    int cpt = 0;

    // For only the third part.
    for (int i = height - 1; i > 2 * height / 3; i--)
    {
        // Inits the sum.
        double sum_line = 0;

        // For each pixel in the middle of the line, add its value to the sum.
        for (int j = min_zone; j < max_zone; j++)
        {
            sum_line += pixels[j + width * i];
        }

        // Computes the average of pixels on the line.
        double moy = sum_line / width;

        if (moy <= threshold)
        {
            for (int i_tmp = last_line; i_tmp > i; i_tmp--)
            {
                for (int j_tmp = 0; j_tmp < width; j_tmp++)
                {
                    pixels[j_tmp + width * i_tmp] = 0;
                    cpt++;
                }
            }

            // Save detected column.
            last_line = i;
        }
    }

    return cpt;
}


// Main function to upgrade image.
//
// surf: initial surface.
// dest: the surface where the result will be stored.
// return: nothing.
void upgrade_exploitation(int debug, SDL_Surface* surf, SDL_Surface** dest)
{
    if (debug)
    {
        printf("\e[1;1H\e[2J");
        printf("%s", "IMPROVED IMAGE PRE-PROCESSING\n");
        printf("%s", "=============================\n");
    }

    // Init.
    int width = surf->w;
    int height = surf->h;
    int pixels[width * height];

    // Converts surface into array.
    get_array_of_pixels(surf, pixels);

    if (debug)
    {
        printf("%s", "Filling image... ");
    }

    // Deals with left part of the image.
    int r1 = fill_left(width, height, pixels);

    // Deals with right part of the image.
    int r2 = fill_right(width, height, pixels);

    // Deals with top part of the image.
    int r3 = fill_top(width, height, pixels);

    // Deals with bottom part of the image.
    int r4 = fill_bottom(width, height, pixels);

    // Compute percentage of modifications.
    int result = 100 * (r1 + r2 + r3 + r4) / (width * height);

    if (debug)
    {
        printf("Rewriting %i%% percent of image... ", result);
        printf(ANSI_COLOR_GREEN "Done.\n\n" ANSI_COLOR_RESET);
    }

    // Saves surface.
    *dest = array_to_surface(width, height, pixels);
}