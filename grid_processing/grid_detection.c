#include "grid_detection.h"

// Constants and global variables.
// =========
// Theta threshold in degrees to keep vertical and horizontal lines.
const double THETA_THRESHOLD = 15;

// Rho and theta thresholds in pixels to merge equivalent lines.
const double RHO_THR_AV = 15;
const double THETA_THR_AV = 15;

// Distance maximal to say two lines can be merged.
const double LIMIT_DISTANCE = 64;

// Distance maximal to say two points can be merged.
const double POINT_RADIUS = 20;

// Colors for fun.
#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_RESET  "\x1b[0m"
// =========

// Keeps only vertical and horizontal lines with list_rho and list_theta.
//
// return: nothing, but fills list_rho_vert, list_theta_vert, list_rho_hori, list_theta_hori.
void orthogonal_filter(struct list* list_rho,struct list* list_theta,
        struct list** list_rho_vert, struct list** list_theta_vert,
                struct list** list_rho_hori, struct list** list_theta_hori)
{
    // Inits pointers to iterates in list_rho and list_theta.
    struct list* l_rho_tmp = list_rho;
    struct list* l_theta_tmp = list_theta;

    while (l_rho_tmp && l_theta_tmp)
    {
        // Gets current values of rho and theta.
        double line_rho = l_rho_tmp->value;
        double line_theta = l_theta_tmp->value;

        // Line vertical ? Checks in interval:
        // [0 - threshold, 0 + threshold].
        if (line_theta > deg_to_rad(-THETA_THRESHOLD)
            && line_theta < deg_to_rad(THETA_THRESHOLD))
        {
            *list_rho_vert = list_insert_head(*list_rho_vert, line_rho);
            *list_theta_vert = list_insert_head(*list_theta_vert, line_theta);
        }

        // Line horizontal ? Checks in interval:
        // [-90 - threshold, -90 + threshold] U [90 - threshold, 90 + threshold].
        else if ((line_theta > deg_to_rad(90 - THETA_THRESHOLD)
                && line_theta < deg_to_rad(90 + THETA_THRESHOLD)) ||
                ((line_theta > deg_to_rad(-90 - THETA_THRESHOLD))
                && (line_theta < deg_to_rad(-90 + THETA_THRESHOLD))))
        {
            *list_rho_hori = list_insert_head(*list_rho_hori, line_rho);
            *list_theta_hori = list_insert_head(*list_theta_hori, line_theta);
        }

        // Goes on the next element.
        l_rho_tmp = l_rho_tmp->next;
        l_theta_tmp = l_theta_tmp->next;
    }
}

// Average clusters of line.
//
//
//
void average_lines(double diag, struct list* list_rho, struct list* list_theta, struct list** dest_rho, struct list** dest_theta)
{
    // Converts lists in arrays.
    int size = list_len(list_rho);
    double arr_rho[size];
    double arr_theta[size];

    // Fills list with rho values.
    int i_tmp = 0;
    struct list* rho_tmp = list_rho;
    struct list* theta_tmp = list_theta;
    while (rho_tmp)
    {
        // Assigns values in arrays.
        arr_rho[i_tmp] = rho_tmp->value;
        arr_theta[i_tmp] = theta_tmp->value;
        i_tmp++;

        // Goes on the next values.
        rho_tmp = rho_tmp->next;
        theta_tmp = theta_tmp->next;
    }

    // For each line.
    for (int index_current_line = 0; index_current_line < size; index_current_line++)
    {
        // Gets the values of theta and rho.
        double rho_cur = arr_rho[index_current_line];
        double theta_cur =  arr_theta[index_current_line];

        // Rho and theta has already been treated, pass.
        if (rho_cur == 0 && theta_cur == -100)
        {
            index_current_line++;
            continue;
        }

        // Finds two points at extremes on the line.
        double x0_cur = cos(theta_cur) * rho_cur;
        double y0_cur = sin(theta_cur) * rho_cur;
        double x1_cur = x0_cur + diag * (-sin(theta_cur));
        double y1_cur = y0_cur + diag * (cos(theta_cur));
        double x2_cur = x0_cur - diag * (-sin(theta_cur));
        double y2_cur = y0_cur - diag * (cos(theta_cur));

        // For each line.
        for (int index_line = 0; index_line < size; index_line++)
        {
            // We want everything except the current line.
            if (index_current_line == index_line)
            {
                index_line++;
                continue;
            }

            // Gets the values of theta and rho, again.
            double rho = arr_rho[index_line];
            double theta =  arr_theta[index_line];

            // Rho and theta has already been treated, pass.
            if (rho == 0 && theta == -100)
            {
                index_line++;
                continue;
            }

            // First filter.
            if (fabs(rho - rho_cur) < RHO_THR_AV && fabs(theta - theta_cur) < deg_to_rad(THETA_THR_AV))
            {
                // Finds two points at extremes on the line.
                double x0 = cos(theta) * rho;
                double y0 = sin(theta) * rho;
                double x1 = x0 + diag * (-sin(theta));
                double y1 = y0 + diag * (cos(theta));
                double x2 = x0 - diag * (-sin(theta));
                double y2 = y0 - diag * (cos(theta));

                // Computes distances.
                int close_1 = sqrt((x1 - x1_cur) * (x1 - x1_cur) + (y1 - y1_cur) * (y1 - y1_cur)) < LIMIT_DISTANCE;
                int close_2 = sqrt((x2 - x2_cur) * (x2 - x2_cur) + (y2 - y2_cur) * (y2 - y2_cur)) < LIMIT_DISTANCE;

                // Second filter.
                if (close_1 && close_2)
                {
                    arr_rho[index_current_line] = (arr_rho[index_current_line] + arr_rho[index_line]) / 2;
                    arr_theta[index_current_line] = (arr_theta[index_current_line] + arr_theta[index_line]) / 2;

                    // Sets that the line has been treated.
                    arr_rho[index_line] = 0;
                    arr_theta[index_line] = -100;
                }
            }
        }
    }

    // Gets all the lines averaged.
    for (int i = 0; i < size; ++i)
    {
        if (arr_theta[i] != -100 && arr_rho[i] != 0)
        {
            *dest_theta = list_insert_head(*dest_theta, arr_theta[i]);
            *dest_rho = list_insert_head(*dest_rho, arr_rho[i]);
        }
    }
}

//
//
//
//
void point_intersection(double surf_diag, double rho1, double theta1, double rho2, double theta2, double* x, double* y)
{
    double x0_1 = cos(theta1) * rho1;
    double y0_1 = sin(theta1) * rho1;
    double x1 = x0_1 + surf_diag * (-sin(theta1));
    double y1 = y0_1 + surf_diag * (cos(theta1));
    double x2 = x0_1 - surf_diag * (-sin(theta1));
    double y2 = y0_1 - surf_diag * (cos(theta1));

    double x0_2 = cos(theta2) * rho2;
    double y0_2 = sin(theta2) * rho2;
    double x3 = x0_2 + surf_diag * (-sin(theta2));
    double y3 = y0_2 + surf_diag * (cos(theta2));
    double x4 = x0_2 - surf_diag * (-sin(theta2));
    double y4 = y0_2 - surf_diag * (cos(theta2));

    double t1 = ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
    double t2 = (x1 * y2 - y1 * x2);
    double t3 = (x3 * y4 - y3 * x4);

    *x = (t2 * (x3 - x4) - (x1 - x2) * t3) / t1;
    *y = (t2 * (y3 - y4) - (y1 - y2) * t3) / t1;
}

//
//
//
//
void get_intersections(double w, double h, double diag, struct list* list_rho, struct list* list_theta, struct list** list_point_x, struct list** list_point_y)
{
    // Inits sorted lines.
    struct list* list_rho_v = list_new();
    struct list* list_theta_v = list_new();
    struct list* list_rho_h = list_new();
    struct list* list_theta_h = list_new();

    // Sorts lines.
    orthogonal_filter(list_rho, list_theta, &list_rho_v, &list_theta_v, &list_rho_h, &list_theta_h);

    // Pointers to iterate lines.
    struct list* list_rho_v_tmp = list_rho_v;
    struct list* list_theta_v_tmp = list_theta_v;

    // For each vertical line.
    while(list_rho_v_tmp)
    {
        // Gets values.
        double rho_v = list_rho_v_tmp->value;
        double theta_v = list_theta_v_tmp->value;

        struct list* list_rho_h_tmp = list_rho_h;
        struct list* list_theta_h_tmp = list_theta_h;

        // For each horizontal line.
        while (list_rho_h_tmp)
        {
            // Gets values.
            double rho_h = list_rho_h_tmp->value;
            double theta_h = list_theta_h_tmp->value;

            // Inits point.
            double x = 0;
            double y = 0;

            // Gets intersection.
            point_intersection(diag, rho_v, theta_v, rho_h, theta_h, &x, &y);

            // Be sure points are on image.
            if (x >= 0 && x <= w && y >= 0 && y <= h)
            {
                *list_point_x = list_insert_head(*list_point_x, x);
                *list_point_y = list_insert_head(*list_point_y, y);
            }

            // Next values.
            list_rho_h_tmp = list_rho_h_tmp->next;
            list_theta_h_tmp = list_theta_h_tmp->next;
        }

        // Next values.
        list_rho_v_tmp = list_rho_v_tmp->next;
        list_theta_v_tmp = list_theta_v_tmp->next;
    }
}

//
//
//
//
void get_extremes_lines(SDL_Surface* surf, double diag, struct list* list_rho, struct list* list_theta)
{
    // Keeps horizontal lines and vertical lines.
    struct list* list_rho_v = list_new();
    struct list* list_theta_v = list_new();
    struct list* list_rho_h = list_new();
    struct list* list_theta_h = list_new();
    orthogonal_filter(list_rho, list_theta, &list_rho_v, &list_theta_v, &list_rho_h, &list_theta_h);

    // Converts lists in array. It will be easier to work with.
    double* arr_rho_v = list_to_array(list_rho_v);
    double* arr_rho_h = list_to_array(list_rho_h);
    double* arr_theta_v = list_to_array(list_theta_v);
    double* arr_theta_h = list_to_array(list_theta_h);

    int max_v_index = max_array_index_abs(arr_rho_v, list_len(list_rho_v));
    int max_h_index = max_array_index_abs(arr_rho_h, list_len(list_rho_h));
    int min_v_index = min_array_index_abs(arr_rho_v, list_len(list_rho_v));
    int min_h_index = min_array_index_abs(arr_rho_h, list_len(list_rho_h));

    double top_rho = arr_rho_h[min_h_index];
    double top_theta = arr_theta_h[min_h_index];
    double bottom_rho = arr_rho_h[max_h_index];
    double bottom_theta = arr_theta_h[max_h_index];
    double left_rho = arr_rho_v[min_v_index];
    double left_theta = arr_theta_v[min_v_index];
    double right_rho = arr_rho_v[max_v_index];
    double right_theta = arr_theta_v[max_v_index];


    double point_top_left_x;
    double point_top_left_y;
    point_intersection(diag, top_rho, top_theta, left_rho, left_theta, &point_top_left_x, &point_top_left_y);

    double point_top_right_x;
    double point_top_right_y;
    point_intersection(diag, top_rho, top_theta, right_rho, right_theta, &point_top_right_x, &point_top_right_y);

    double point_bottom_left_x;
    double point_bottom_left_y;
    point_intersection(diag, bottom_rho, bottom_theta, left_rho, left_theta, &point_bottom_left_x, &point_bottom_left_y);

    double point_bottom_right_x;
    double point_bottom_right_y;
    point_intersection(diag, bottom_rho, bottom_theta, right_rho, right_theta, &point_bottom_right_x, &point_bottom_right_y);

    struct list* list_points_x = list_new();
    struct list* list_points_y = list_new();

    if (0)
    {
        list_points_x = list_insert_head(list_points_x, point_top_left_x);
        list_points_x = list_insert_head(list_points_x, point_top_right_x);
        list_points_x = list_insert_head(list_points_x, point_bottom_left_x);
        list_points_x = list_insert_head(list_points_x, point_bottom_right_x);

        list_points_y = list_insert_head(list_points_y, point_top_left_y);
        list_points_y = list_insert_head(list_points_y, point_top_right_y);
        list_points_y = list_insert_head(list_points_y, point_bottom_left_y);
        list_points_y = list_insert_head(list_points_y, point_bottom_right_y);

        draw_points_on_window(list_points_x, list_points_y, surf);
    }

    // Gets coordinates of all cells.
    struct list* coordinates_x = list_new();
    struct list* coordinates_y = list_new();

    double step_h = sqrt((point_top_left_x - point_top_right_x) * (point_top_left_x - point_top_right_x) + (point_top_left_y - point_top_right_y) * (point_top_left_y - point_top_right_y)) / 9;
    double step_v = sqrt((point_top_left_x - point_bottom_left_x) * (point_top_left_x - point_bottom_left_x) + (point_top_left_y - point_bottom_left_y) * (point_top_left_y - point_bottom_left_y)) / 9;

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

            // char filepath[100];
            // snprintf(filepath, sizeof(filepath), "flex_%i%i.png", cpt_x, cpt_y);
            // IMG_SavePNG(new_surf, filepath);

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
    list_destroy(list_rho_v);
    list_destroy(list_theta_v);
    list_destroy(list_rho_h);
    list_destroy(list_theta_h);
    free(arr_rho_v);
    free(arr_rho_h);
}

//
//
//
//
void average_points(struct list* list_x, struct list* list_y, struct list** dest_x,  struct list** dest_y)
{
    // Converts lists in arrays.
    int size = list_len(list_x);
    double arr_x[size];
    double arr_y[size];

    // Fills list with rho values.
    int i_tmp = 0;
    struct list* x_tmp = list_x;
    struct list* y_tmp = list_y;
    while (x_tmp)
    {
        // Assigns values in arrays.
        arr_x[i_tmp] = x_tmp->value;
        arr_y[i_tmp] = y_tmp->value;
        i_tmp++;

        // Goes on the next values.
        x_tmp = x_tmp->next;
        y_tmp = y_tmp->next;
    }

    // For each point.
    for (int index_curr = 0; index_curr < size; index_curr++)
    {
        // Gets the values of x and y.
        double x_curr = arr_x[index_curr];
        double y_curr =  arr_y[index_curr];

        // x and y has already been treated, pass.
        if (x_curr == -100 && y_curr == -100)
        {
            index_curr++;
            continue;
        }

        // For each point.
        for (int index = 0; index < size; index++)
        {
            // We want everything except the current point.
            if (index_curr == index)
            {
                index++;
                continue;
            }

            // Gets the values of x and y, again.
            double x = arr_x[index];
            double y = arr_y[index];

            if (x == -100 && y == -100)
            {
                index++;
                continue;
            }

            // First filter.
            if (sqrt((x_curr - x) * (x_curr - x) + (y_curr - y) * (y_curr - y)) < POINT_RADIUS)
            {
                arr_x[index_curr] = (x_curr + x) / 2;
                arr_y[index_curr] = (y_curr + y) / 2;

                // Sets that the point has been treated.
                arr_x[index] = -100;
                arr_y[index] = -100;
            }
        }
    }

    // Gets all the points averaged.
    for (int i = 0; i < size; i++)
    {
        if (arr_x[i] != -100 && arr_y[i] != -100)
        {
            *dest_x = list_insert_head(*dest_x, arr_x[i]);
            *dest_y = list_insert_head(*dest_y, arr_y[i]);
        }
    }
}

//
//
//
//
double distance_horizontal_lines(double rho1, double theta1, double rho2, double theta2, double width)
{
    double x = width;
    double y1 = (rho1 - x * cos(theta1)) / sin(theta1);
    double y2 = (rho2 - x * cos(theta2)) / sin(theta2);

    return fabs(y2 - y1);
}

// Full process to detect the grid and the digits.
//
// debug: set to one for additional information during the process.
// list_rho: initial list of rhos.
// list_rho: initial list of thetas.
// diag: diagonal of the image.
// surf: surface from the image.
// return: nothing.
void grid_detection(int debug, struct list* list_rho, struct list* list_theta, double diag, SDL_Surface* surf)
{
    // DEBUG.
    if (debug)
    {
        printf("Averaging lines... ");
    }

    // AVERAGING LINES.
    // ==========================================
    struct list* list_rho_av = list_new();
    struct list* list_theta_av = list_new();
    average_lines(diag, list_rho, list_theta, &list_rho_av, &list_theta_av);
    // ==========================================

    // DEBUG.
    if (debug)
    {
        // Some information about averaging.
        printf("Average %i lines to %i lines. ", list_len(list_rho), list_len(list_rho_av));

        // Show window with the new lines averaged.
        draw_lines_on_window(list_rho_av, list_theta_av, surf, diag);

        // Informs the process is done.
        printf(ANSI_COLOR_GREEN "Done.\n" ANSI_COLOR_RESET);

        // Some information about points.
        printf("Finding intersections... ");
    }

    // GETS ALL INTERSECTIONS OF ALL LINES (INCLUDES ORTHOGONAL FILTER)
    // ===================================
    struct list* list_point_x = list_new();
    struct list* list_point_y = list_new();
    get_intersections(surf->w, surf->h, diag, list_rho_av, list_theta_av, &list_point_x, &list_point_y);
    // ===================================

    // DEBUG.
    if (debug)
    {
        // Show window with the points.
        draw_points_on_window(list_point_x, list_point_y, surf);

        // Informs the process is done.
        printf("%i intersections found. ", list_len(list_point_x));
        printf(ANSI_COLOR_GREEN "Done.\n" ANSI_COLOR_RESET);

        // Some information about points.
        printf("Averaging points... ");
    }

    // AVERAGING OF POINTS (TO BE SURE)
    // ===================================

    struct list* list_x_av = list_new();
    struct list* list_y_av = list_new();
    average_points(list_point_x, list_point_y, &list_x_av, &list_y_av);
    // ===================================

    // DEBUG.
    if (debug)
    {
        // Show window with the points.
        draw_points_on_window(list_x_av, list_y_av, surf);

        // Informs the process is done.
        printf("Average %i points to %i points. ", list_len(list_point_x), list_len(list_x_av));
        printf(ANSI_COLOR_GREEN "Done.\n" ANSI_COLOR_RESET);
    }

    get_extremes_lines(surf, diag, list_rho, list_theta);

    // Frees memory.
    list_destroy(list_rho_av);
    list_destroy(list_theta_av);
    list_destroy(list_point_x);
    list_destroy(list_point_y);
    list_destroy(list_x_av);
    list_destroy(list_y_av);

}