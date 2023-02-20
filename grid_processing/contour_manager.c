#include "contour_manager.h"
#include <float.h>

/// @brief Gets the polygon area from a set of points.
///
/// @param point_x Initial set of the x coordinates of the points.
/// @param point_y Initial set of the y coordinates of the points.
/// @return the polygon area
double get_polygon_area(struct list* point_x, struct list* point_y)
{
    // Transforms lists into array. It will be easier to work with.
    int size = list_len(point_x);
    double* arr_x = list_to_array(point_x);
    double* arr_y = list_to_array(point_y);

    // Parameters.
    double area = 0;
    int j = size - 1;

    // Computes area.
    for (int i = 0; i < size; i++)
    {
        area += (arr_x[j] + arr_x[i]) * (arr_y[j] - arr_y[i]);
        j = i;
    }

    return fabs(area / 2);
}

/// @brief Gets the convex hull from a set of points. Implementing Convex Hull Algorithm
///
/// @param point_x Initial set of the x coordinates of the points.
/// @param point_y Initial set of the y coordinates of the points.
/// @param dest_x Destination set of the x coordinates of the points.
/// @param dest_y Destination set of the y coordinates of the points.
/// @return nothing
void convex_hull(struct list* point_x, struct list* point_y, struct list** dest_x, struct list** dest_y)
{
    // Transforms lists into array. It will be easier to work with.
    int size = list_len(point_x);
    double* arr_x = list_to_array(point_x);
    double* arr_y = list_to_array(point_y);

    // Gets the minimum point.
    int min_index = 0;
    for (int i = 1; i < size; i++)
    {
        if (arr_x[i] < arr_x[min_index] && arr_y[i] < arr_y[min_index])
            min_index = i;
    }

    // Our first point is the point with the smallest coordinates.
    // So we add it to the destination lists.
    *dest_x = list_insert_head(*dest_x, arr_x[min_index]);
    *dest_y = list_insert_head(*dest_y, arr_y[min_index]);

    // Parameters.
    int *pixel_is_visited_x = calloc(size, sizeof(int));
    int curr_index = min_index;

    // Main algorithm.
    while(1)
    {
        // Computes Q.
        int q = (curr_index + 1) % size;

        for (int i = 0; i < size; i++)
        {
            // We don't want this point.
            if (i == curr_index)
                continue;

            // Gets the direction of the point.
            int x1l = arr_x[curr_index];
            int y1l = arr_y[curr_index];
            int x2i = arr_x[i];
            int y2i = arr_y[i];
            int x3q = arr_x[q];
            int y3q = arr_y[q];
            int dir = (x3q - x1l) * (y2i - y1l) - (x2i - x1l) * (y3q - y1l);

            // Checks direction.
            if (dir > 0 || (dir == 0 && distance(x2i, y2i, x1l, y1l) > distance(x3q, y3q, x1l, y1l)))
                q = i;
        }

        curr_index = q;
        if (curr_index == min_index || pixel_is_visited_x[q] == 1)
            break;

        // Saves pixel coordinates.
        *dest_x = list_insert_head(*dest_x, arr_x[q]);
        *dest_y = list_insert_head(*dest_y, arr_y[q]);
        pixel_is_visited_x[q] = 1;
    }
}

/// @brief Gets the points composing the largest shape in image. Uses Flood Fill algorithm.
///
/// @param width width of the image source.
/// @param height height of the image source.
/// @param pixels array of binary pixels from the image source.
/// @param save_point_x destination for x coordinate of the found points.
/// @param save_point_y destination for y coordinate of the found points.
/// @return nothing
void get_points_max_shape(int width, int height, int *pixels, struct list** save_point_x, struct list** save_point_y)
{
    // Parameters.
    struct list* tmp_save_point_x = list_new();
    struct list* tmp_save_point_y = list_new();
    int *pixel_is_visited_x = calloc(width * height, sizeof(int));
    double max_area = 0;

    // For each pixel in image.
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // If pixel is black, pass.
            if (pixels[x + width * y] == 0)
                continue;

            // If pixel has already been treated, pass.
            if (pixel_is_visited_x[x + width * y])
                continue;

            // Creates a stack and insert the first element: the pixel (x, y).
            struct list* stack_x = list_new();
            struct list* stack_y = list_new();
            stack_x = list_insert_head(stack_x, x);
            stack_y = list_insert_head(stack_y, y);

            // Main algorithm.
            while (!list_is_empty(stack_x))
            {
                // Gets first element and pop it from stack.
                int x_curr = (int) stack_x->value;
                int y_curr = (int) stack_y->value;
                stack_x = list_remove_head(stack_x);
                stack_y = list_remove_head(stack_y);

                // If pixel has already been treated, pass.
                if (pixel_is_visited_x[x_curr + width * y_curr] == 1)
                    continue;

                // Pixel is visited, so update list and save it.
                pixel_is_visited_x[x_curr + width * y_curr] = 1;
                tmp_save_point_x = list_insert_head(tmp_save_point_x, x_curr);
                tmp_save_point_y = list_insert_head(tmp_save_point_y, y_curr);

                // Left.
                if (x_curr - 1 >= 0 && pixels[(x_curr - 1) + width * (y_curr)] == 1)
                {
                    stack_x = list_insert_head(stack_x, x_curr - 1);
                    stack_y = list_insert_head(stack_y, y_curr);
                }

                // Right
                if (x_curr + 1 < width && pixels[(x_curr + 1) + width * (y_curr)] == 1)
                {
                    stack_x = list_insert_head(stack_x, x_curr + 1);
                    stack_y = list_insert_head(stack_y, y_curr);
                }

                // Up.
                if (y_curr - 1 >= 0 && pixels[(x_curr) + width * (y_curr - 1)] == 1)
                {
                    stack_x = list_insert_head(stack_x, x_curr);
                    stack_y = list_insert_head(stack_y, y_curr - 1);
                }

                // Down.
                if (y_curr + 1 < height && pixels[(x_curr) + width * (y_curr + 1)] == 1)
                {
                    stack_x = list_insert_head(stack_x, x_curr);
                    stack_y = list_insert_head(stack_y, y_curr + 1);
                }
            }

            // At the end of the loop, we have a big set of points.
            // All points represent an area. Let's get the points of the contour.
            // We implement the Convex Hull Algorithm.
            struct list* hull_points_x = list_new();
            struct list* hull_points_y = list_new();
            convex_hull(tmp_save_point_x, tmp_save_point_y, &hull_points_x, &hull_points_y);

            // Gets area of the convex hull.
            double area = get_polygon_area(hull_points_x, hull_points_y);

            // Checks area.
            if (area > max_area)
            {
                max_area = area;

                // Erases the points.
                list_destroy(*save_point_x);
                list_destroy(*save_point_y);
                *save_point_x = list_new();
                *save_point_y = list_new();

                // Transfers tempo in right lists.
                while (!list_is_empty(hull_points_x))
                {
                    double save_x = hull_points_x->value;
                    double save_y = hull_points_y->value;
                    hull_points_x = list_remove_head(hull_points_x);
                    hull_points_y = list_remove_head(hull_points_y);
                    *save_point_x = list_insert_head(*save_point_x, save_x);
                    *save_point_y = list_insert_head(*save_point_y, save_y);
                }
            }

            // Frees memory.
            list_destroy(tmp_save_point_x);
            list_destroy(tmp_save_point_y);
            list_destroy(hull_points_x);
            list_destroy(hull_points_y);
            tmp_save_point_x = list_new();
            tmp_save_point_y = list_new();
        }
    }

    // Frees memory.
    list_destroy(tmp_save_point_x);
    list_destroy(tmp_save_point_y);
    free(pixel_is_visited_x);
}

/// @brief Finds the corners of the rectangle.
///
/// @param point_x Initial set of the x coordinates of the points.
/// @param point_y Initial set of the y coordinates of the points.
/// @param dest_x Destination set of the x coordinates of the points.
/// @param dest_y Destination set of the x coordinates of the points.
/// @param size Size of arrays
void find_corners_of_rectangle(double* arr_x, double* arr_y, double *dest_x, double *dest_y, double size)
{
    // Inits the corners.
    double top_left_x = 0;
    double top_left_y = 0;
    double top_right_x = 0;
    double top_right_y = 0;
    double bottom_right_x = 0;
    double bottom_right_y = 0;
    double bottom_left_x = 0;
    double bottom_left_y = 0;

    // Inits the average boundaries of image.
    double min_x = 100000;
    double min_y = 100000;
    double max_x = 0;
    double max_y = 0;

    // Gets the average boundaries of image.
    for (int i = 1; i < size; i++)
    {
        if (arr_x[i] < min_x)
            min_x = arr_x[i];

        if (arr_y[i] < min_y)
            min_y = arr_y[i];

        if (arr_x[i] > max_x)
            max_x = arr_x[i];

        if (arr_y[i] > max_y)
            max_y = arr_y[i];
    }

    // Inits the distances.
    double dist_bottom_right_tmp = 100000;
    double dist_bottom_left_tmp = 100000;
    double dist_top_left_tmp = 100000;
    double dist_top_right_tmp = 100000;

    // Attributes the points with the smallest distances with the boundaries.
    for (int i = 0; i < size; i++)
    {
        // Computes distances.
        double dist_bottom_right = distance(arr_x[i], arr_y[i], max_x, max_y);
        double dist_bottom_left = distance(arr_x[i], arr_y[i], min_x, max_y);
        double dist_top_right = distance(arr_x[i], arr_y[i], max_x, min_y);
        double dist_top_left = distance(arr_x[i], arr_y[i], min_x, min_y);

        // Bottom right.
        if (dist_bottom_right < dist_bottom_right_tmp)
        {
            dist_bottom_right_tmp = dist_bottom_right;
            bottom_right_x = arr_x[i];
            bottom_right_y = arr_y[i];
        }

        // Bottom left.
        if (dist_bottom_left < dist_bottom_left_tmp)
        {
            dist_bottom_left_tmp = dist_bottom_left;
            bottom_left_x = arr_x[i];
            bottom_left_y = arr_y[i];
        }

        // Top left.
        if (dist_top_left < dist_top_left_tmp)
        {
            dist_top_left_tmp = dist_top_left;
            top_left_x = arr_x[i];
            top_left_y = arr_y[i];
        }

        // Top right.
        if (dist_top_right < dist_top_right_tmp)
        {
            dist_top_right_tmp = dist_top_right;
            top_right_x = arr_x[i];
            top_right_y = arr_y[i];
        }
    }

    // Saves all the points.
    // Now we have the corners.
    dest_x[0] = top_left_x;
    dest_y[0] = top_left_y;
    dest_x[1] = top_right_x;
    dest_y[1] = top_right_y;
    dest_x[2] = bottom_right_x;
    dest_y[2] = bottom_right_y;
    dest_x[3] = bottom_left_x;
    dest_y[3] = bottom_left_y;
}



// tmp
void TMP_extremes_lines_and_cell_extraction(SDL_Surface* surf, double corners_x[], double corners_y[])
{
    double point_top_left_x = corners_x[0];
    double point_top_left_y = corners_y[0];
    double point_top_right_x = corners_x[1];
    double point_top_right_y = corners_y[1];
    double point_bottom_left_x = corners_x[3];
    double point_bottom_left_y = corners_y[3];
    // double point_bottom_right_x = corners_x[3];
    // double point_bottom_right_y = corners_y[3];

    struct list* coordinates_x = list_new();
    struct list* coordinates_y = list_new();

    double step_h = sqrt((point_top_left_x - point_top_right_x) * (point_top_left_x - point_top_right_x)
                         + (point_top_left_y - point_top_right_y) * (point_top_left_y - point_top_right_y)) / 9;
    double step_v = sqrt((point_top_left_x - point_bottom_left_x) * (point_top_left_x - point_bottom_left_x)
                         + (point_top_left_y - point_bottom_left_y) * (point_top_left_y - point_bottom_left_y)) / 9;

    double tmp_x = point_top_left_x;
    for (int i = 0; i < 9; i++)
    {
        coordinates_x = list_insert_head(coordinates_x, tmp_x);
        tmp_x += step_h;
    }

    double tmp_y = point_top_left_y;
    for (int i = 0; i < 9; i++)
    {
        coordinates_y = list_insert_head(coordinates_y, tmp_y);
        tmp_y += step_v;
    }

    int cpt_y = 0;

    // Creates surfaces.
    struct list* y_tmp = coordinates_y;

    while (y_tmp)
    {
        struct list* x_tmp = coordinates_x;
        int cpt_x = 0;

        while (x_tmp)
        {
            SDL_Rect rect;
            rect.h = step_h;
            rect.w = step_v;
            rect.x = x_tmp->value;
            rect.y = y_tmp->value;

            SDL_Surface* new_surf = SDL_CreateRGBSurfaceWithFormat(0, step_h, step_v, 32, SDL_PIXELFORMAT_RGBA32);
            SDL_BlitSurface(surf, &rect, new_surf, NULL);

            char filepath[100];
            snprintf(filepath, sizeof(filepath), "img/%i%i.png", cpt_x, cpt_y);
            IMG_SavePNG(new_surf, filepath);

            cpt_x++;
            x_tmp = x_tmp->next;
            SDL_FreeSurface(new_surf);
        }

        cpt_y++;
        y_tmp = y_tmp->next;
    }

    // Frees memory.
    list_destroy(coordinates_y);
    list_destroy(coordinates_x);
}

//
//
//
//
void four_points(double arr_x[], double arr_y[], double dest_x[], double dest_y[], int size)
{
    // Compute the convex hull using the Graham scan algorithm
    // and store the result in the arr_x and arr_y arrays
    // (implementation of this step is not shown here)

    // Initialize variables to hold the four extreme points
    double p1_x = 0, p1_y = 0;
    double p2_x = 0, p2_y = 0;
    double p3_x = 0, p3_y = 0;
    double p4_x = 0, p4_y = 0;

    // Find the two points that are farthest apart on the convex hull
    double max_dist = 0;
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            double dist = sqrt(pow(arr_x[i] - arr_x[j], 2) + pow(arr_y[i] - arr_y[j], 2));
            if (dist > max_dist) {
                max_dist = dist;
                p1_x = arr_x[i];
                p1_y = arr_y[i];
                p2_x = arr_x[j];
                p2_y = arr_y[j];
            }
        }
    }

    // Find the point that is farthest away from the line connecting p1 and p2
    double max_dist_1 = 0;
    double line_slope = (p2_y - p1_y) / (p2_x - p1_x);
    double line_intercept = p1_y - line_slope * p1_x;
    for (int i = 0; i < size; i++) {
        double dist = fabs((arr_y[i] - line_slope * arr_x[i] - line_intercept) / sqrt(pow(line_slope, 2) + 1));
        if (dist > max_dist_1) {
            max_dist_1 = dist;
            p3_x = arr_x[i];
            p3_y = arr_y[i];
        }
    }

    // Find the point that is farthest away from both line segments p1-p3 and p2-p3
    double max_dist_2 = 0;
    for (int i = 0; i < size; i++) {
        // double dist1 = fabs((arr_y[i] - line_slope * arr_x[i] - line_intercept) / sqrt(pow(line_slope, 2) + 1));
        // double dist2 = 0;
        for (int j = 0; j < size; j++) {
            if ((arr_x[j] == p1_x && arr_y[j] == p1_y) || (arr_x[j] == p2_x && arr_y[j] == p2_y)) {
                continue;
            }
            double line1_slope = (p3_y - p1_y) / (p3_x - p1_x);
            double line1_intercept = p3_y - line1_slope * p3_x;
            double dist_line1 = fabs((arr_y[i] - line1_slope * arr_x[i] - line1_intercept) / sqrt(pow(line1_slope, 2) + 1));

            double line2_slope = (p3_y - p2_y) / (p3_x - p2_x);
            double line2_intercept = p3_y - line2_slope * p3_x;
            double dist_line2 = fabs((arr_y[i] - line2_slope * arr_x[i] - line2_intercept) / sqrt(pow(line2_slope, 2) + 1));
            double dist = fmin(dist_line1, dist_line2);
            if (dist > max_dist_2) {
                max_dist_2 = dist;
                p4_x = arr_x[i];
                p4_y = arr_y[i];
            }
        }
    }

    // Copy the x and y coordinates of the four extreme points into the dest_x and dest_y arrays
    dest_x[0] = p1_x;
    dest_y[0] = p1_y;
    dest_x[1] = p2_x;
    dest_y[1] = p2_y;
    dest_x[2] = p3_x;
    dest_y[2] = p3_y;
    dest_x[3] = p4_x;
    dest_y[3] = p4_y;
}

/// @brief Main function to detect the coordinates of the four corners of sudoku
///
/// @param surf Initial surface processed.
/// @param corners_x Destination array to store x coordinates of points.
/// @param corners_y Destination array to store y coordinates of points.
void get_max_points_rect(SDL_Surface* surf, double *corners_x, double *corners_y)
{
    // Gets parameters of surface.
    int width = surf->w;
    int height = surf->h;
    int arr_pixels[width * height];
    get_array_of_pixels(surf, arr_pixels);
    // double diag = sqrt(width * width + height * height);

    // FIND COORDINATES OF MAX SHAPE IN IMAGE
    // ======================================
    struct list* shape_point_x = list_new();
    struct list* shape_point_y = list_new();
    get_points_max_shape(width, height, arr_pixels, &shape_point_x, &shape_point_y);
    // ======================================

    // FIND COORDINATES OF CORNERS
    // ======================================
    double dest_x[4];
    double dest_y[4];
    double *arr_x = list_to_array(shape_point_x);
    double *arr_y = list_to_array(shape_point_y);
    four_points(arr_x, arr_y, dest_x, dest_y, list_len(shape_point_x));
    free(arr_x);
    free(arr_y);
    // ======================================

    // SORT CORNERS
    // ======================================
    find_corners_of_rectangle(dest_x, dest_y, corners_x, corners_y, 4);
    // ======================================

    /*
    for (int i = 0; i < 4; i++)
    {
        printf("%f,%f\n", dest_x[i], dest_y[i]);
    }
    */
    /*
    struct list* point_x = list_new();
    struct list* point_y = list_new();
    point_x = list_insert_head(point_x, dest_x[0]);
    point_x = list_insert_head(point_x, dest_x[1]);
    point_x = list_insert_head(point_x, dest_x[2]);
    point_x = list_insert_head(point_x, dest_x[3]);
    point_y = list_insert_head(point_y, dest_y[0]);
    point_y = list_insert_head(point_y, dest_y[1]);
    point_y = list_insert_head(point_y, dest_y[2]);
    point_y = list_insert_head(point_y, dest_y[3]);
    draw_points_on_window(point_x, point_y, surf);
    */
    // TMP - Cell extraction
    TMP_extremes_lines_and_cell_extraction(surf, corners_x, corners_y);

    // DEBUG
    /*
    struct list* point_x = list_new();
    struct list* point_y = list_new();
    point_x = list_insert_head(point_x, corners_x[0]);
    point_x = list_insert_head(point_x, corners_x[1]);
    point_x = list_insert_head(point_x, corners_x[2]);
    point_x = list_insert_head(point_x, corners_x[3]);
    point_y = list_insert_head(point_y, corners_y[0]);
    point_y = list_insert_head(point_y, corners_y[1]);
    point_y = list_insert_head(point_y, corners_y[2]);
    point_y = list_insert_head(point_y, corners_y[3]);
    draw_points_on_window(point_x, point_y, surf);
    */

    // Frees memory.
    list_destroy(shape_point_x);
    list_destroy(shape_point_y);
}