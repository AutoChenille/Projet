#include "contour_manager.h"

//
//
//
//
double get_polygon_area(struct list* point_x, struct list* point_y)
{
    int size = list_len(point_x);

    double* arr_x = list_to_array(point_x);
    double* arr_y = list_to_array(point_y);

    double area = 0;
    int j = size - 1;

    for (int i = 0; i < size; i++)
    {
        area += (arr_x[j] + arr_x[i]) * (arr_y[j] - arr_y[i]);
        j = i;
    }

    return fabs(area / 2);
}

// Gets the convex hull from a set of points.
//
//
//
void convex_hull(struct list* point_x, struct list* point_y, struct list** dest_x, struct list** dest_y)
{
    int size = list_len(point_x);

    double* arr_x = list_to_array(point_x);
    double* arr_y = list_to_array(point_y);

    // Find minimum point
    int min_index = 0;
    for (int i = 1; i < size; i++)
    {
        if (arr_x[i] < arr_x[min_index] && arr_y[i] < arr_y[min_index])
        {
            min_index = i;
        }
    }

    *dest_x = list_insert_head(*dest_x, arr_x[min_index]);
    *dest_y = list_insert_head(*dest_y, arr_y[min_index]);

    int *pixel_is_visited_x = calloc(size, sizeof(int));

    int curr_index = min_index;

    while(1)
    {
        int q = (curr_index + 1) % size;

        for (int i = 0; i < size; i++)
        {
            if (i == curr_index)
                continue;

            // Direction.
            int x1l = arr_x[curr_index];
            int y1l = arr_y[curr_index];
            int x2i = arr_x[i];
            int y2i = arr_y[i];
            int x3q = arr_x[q];
            int y3q = arr_y[q];

            int p1_x = x3q - x1l;
            int p1_y = y3q - y1l;
            int p2_x = x2i - x1l;
            int p2_y = y2i - y1l;

            int direction = p1_x * p2_y - p2_x * p1_y;

            if (direction > 0 || (direction == 0 && distance(x2i, y2i, x1l, y1l) > distance(x3q, y3q, x1l, y1l)))
                q = i;
        }

        curr_index = q;
        if (curr_index == min_index || pixel_is_visited_x[q] == 1)
            break;

        *dest_x = list_insert_head(*dest_x, arr_x[q]);
        *dest_y = list_insert_head(*dest_y, arr_y[q]);
        pixel_is_visited_x[q] = 1;
    }
}

// Gets the points composing the largest shape in image.
//
//
//
void get_points_max_shape(SDL_Surface* surf, struct list** save_point_x, struct list** save_point_y)
{
    // Gets parameters of surface.
    int width = surf->w;
    int height = surf->h;

    int arr_pixels[width * height];
    get_array_of_pixels(surf, arr_pixels);

    int *pixel_is_visited_x = calloc(width * height, sizeof(int));

    // Area
    double max_area = 0;

    // Contours tempo
    struct list* tmp_save_point_x = list_new();
    struct list* tmp_save_point_y = list_new();

    // For each pixel in image;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (arr_pixels[x + width * y] == 0)
                continue;

            if (pixel_is_visited_x[x + width * y])
                continue;

            struct list* list_stack_x = list_new();
            struct list* list_stack_y = list_new();

            list_stack_x = list_insert_head(list_stack_x, x);
            list_stack_y = list_insert_head(list_stack_y, y);

            // Flood fill algorithm.
            while (!list_is_empty(list_stack_x))
            {
                // Gets first element;
                int x_curr = (int) list_stack_x->value;
                int y_curr = (int) list_stack_y->value;

                // Removes element from stack.
                list_stack_x = list_remove_head(list_stack_x);
                list_stack_y = list_remove_head(list_stack_y);

                if (pixel_is_visited_x[x_curr + width * y_curr] == 1)
                {
                    continue;
                }

                // Pixel is visited.
                pixel_is_visited_x[x_curr + width * y_curr] = 1;

                // Save pixel.
                tmp_save_point_x = list_insert_head(tmp_save_point_x, x_curr);
                tmp_save_point_y = list_insert_head(tmp_save_point_y, y_curr);

                if (x_curr - 1 >= 0 && arr_pixels[(x_curr - 1) + width * (y_curr)] == 1)
                {
                    list_stack_x = list_insert_head(list_stack_x, x_curr - 1);
                    list_stack_y = list_insert_head(list_stack_y, y_curr);
                }

                if (x_curr + 1 < width && arr_pixels[(x_curr + 1) + width * (y_curr)] == 1)
                {
                    list_stack_x = list_insert_head(list_stack_x, x_curr + 1);
                    list_stack_y = list_insert_head(list_stack_y, y_curr);
                }

                if (y_curr - 1 >= 0 && arr_pixels[(x_curr) + width * (y_curr - 1)] == 1)
                {
                    list_stack_x = list_insert_head(list_stack_x, x_curr);
                    list_stack_y = list_insert_head(list_stack_y, y_curr - 1);
                }

                if (y_curr + 1 < height && arr_pixels[(x_curr) + width * (y_curr + 1)] == 1)
                {
                    list_stack_x = list_insert_head(list_stack_x, x_curr);
                    list_stack_y = list_insert_head(list_stack_y, y_curr + 1);
                }
            }

            struct list* hull_points_x = list_new();
            struct list* hull_points_y = list_new();
            convex_hull(tmp_save_point_x, tmp_save_point_y, &hull_points_x, &hull_points_y);
            double area = get_polygon_area(hull_points_x, hull_points_y);

            if (area > max_area)
            {
                max_area = area;

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

// Returns four points
void find_corners_of_rectangle(struct list* point_x, struct list* point_y, struct list** dest_x, struct list** dest_y)
{
    double size = list_len(point_x);

    double* arr_x = list_to_array(point_x);
    double* arr_y = list_to_array(point_y);

    double top_left_x = 0;
    double top_left_y = 0;
    double top_right_x = 0;
    double top_right_y = 0;
    double bottom_right_x = 0;
    double bottom_right_y = 0;
    double bottom_left_x = 0;
    double bottom_left_y = 0;

    double min_x = 100000;
    double min_y = 100000;
    double max_x = 0;
    double max_y = 0;

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

    double dist_bottom_right_tmp = 100000;
    double dist_bottom_left_tmp = 100000;
    double dist_top_left_tmp = 100000;
    double dist_top_right_tmp = 100000;

    for (int i = 0; i < size; i++)
    {
        double dist_bottom_right = distance(arr_x[i], arr_y[i], max_x, max_y);
        double dist_bottom_left = distance(arr_x[i], arr_y[i], min_x, max_y);
        double dist_top_right = distance(arr_x[i], arr_y[i], max_x, min_y);
        double dist_top_left = distance(arr_x[i], arr_y[i], min_x, min_y);

        if (dist_bottom_right < dist_bottom_right_tmp)
        {
            dist_bottom_right_tmp = dist_bottom_right;
            bottom_right_x = arr_x[i];
            bottom_right_y = arr_y[i];
        }

        if (dist_bottom_left < dist_bottom_left_tmp)
        {
            dist_bottom_left_tmp = dist_bottom_left;
            bottom_left_x = arr_x[i];
            bottom_left_y = arr_y[i];
        }

        if (dist_top_left < dist_top_left_tmp)
        {
            dist_top_left_tmp = dist_top_left;
            top_left_x = arr_x[i];
            top_left_y = arr_y[i];
        }

        if (dist_top_right < dist_top_right_tmp)
        {
            dist_top_right_tmp = dist_top_right;
            top_right_x = arr_x[i];
            top_right_y = arr_y[i];
        }
    }

    *dest_x = list_insert_head(*dest_x, top_left_x);
    *dest_y = list_insert_head(*dest_y, top_left_y);
    *dest_x = list_insert_head(*dest_x, top_right_x);
    *dest_y = list_insert_head(*dest_y, top_right_y);
    *dest_x = list_insert_head(*dest_x, bottom_right_x);
    *dest_y = list_insert_head(*dest_y, bottom_right_y);
    *dest_x = list_insert_head(*dest_x, bottom_left_x);
    *dest_y = list_insert_head(*dest_y, bottom_left_y);
}

void get_max_points_rect(SDL_Surface* surf, struct list** corners_x, struct list** corners_y)
{
    return;
    // FIND COORDINATES OF MAX SHAPE IN IMAGE
    // ======================================
    struct list* shape_point_x = list_new();
    struct list* shape_point_y = list_new();
    get_points_max_shape(surf, &shape_point_x, &shape_point_y);
    // ======================================

    // FIND COORDINATES OF CORNERS
    // ======================================
    find_corners_of_rectangle(shape_point_x, shape_point_y, &(*corners_x), &(*corners_y));
    // ======================================

    // Frees memory.
    list_destroy(shape_point_x);
    list_destroy(shape_point_y);
}