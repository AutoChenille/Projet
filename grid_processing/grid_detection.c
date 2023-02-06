#include "grid_detection.h"

// Constants and global variables.
// =========
// Theta threshold in degrees to keep vertical and horizontal lines.
const double THETA_THRESHOLD = 15;

// Rho and theta thresholds in pixels to merge equivalent lines.
const double RHO_THR_AV = 25;
const double THETA_THR_AV = 25;

// Distance maximal to say two lines can be merged.
const double LIMIT_DISTANCE = 64;

// Distance maximal to say two points can be merged.
const double POINT_RADIUS = 20;
// =========

/// @brief Keeps only vertical and horizontal lines with list_rho and list_theta.
///
/// \param list_rho Initial list or rhos.
/// \param list_theta Initial list or thetas.
/// \param list_rho_vert Destination list for vertical rhos.
/// \param list_theta_vert Destination list for vertical thetas.
/// \param list_rho_hori Destination list for horizontal rhos.
/// \param list_theta_hori Destination list for horizontal thetas.
/// @return nothing, but fills list_rho_vert, list_theta_vert, list_rho_hori, list_theta_hori.
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

/// @brief Average clusters of line.
///
/// @param diag Diagonal of surface.
/// @param list_rho Rhos of lines.
/// @param list_theta Thetas of lines.
/// @param dest_rho Rhos of new averaged lines.
/// @param dest_theta Thetas of new averaged lines.
/// @return nothing.
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

/// @brief Computes the coordinates of the intersection of two lines.
///
/// \param surf_diag Diagonal of surface.
/// \param rho1 Rho of first line.
/// \param theta1 Theta of first line.
/// \param rho2 Rho of second line.
/// \param theta2 Theta of second line.
/// \param x Intersection point x.
/// \param y Intersection point y
/// @return nothing.
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

/// @brief Gets all intersections of all lines.
///
/// \param w Width of surface.
/// \param h Height of surface
/// \param diag Diagonal of surface.
/// \param list_rho Rhos of lines
/// \param list_theta Thetas of lines
/// \param list_point_x Destination list with the x coordinates of points.
/// \param list_point_y Destination list with the y coordinates of points.
/// @return nothing.
void get_intersections(double w, double h, double diag, struct list* list_rho,
        struct list* list_theta, struct list** list_point_x, struct list** list_point_y)
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

/// @brief Creates an hypothetical perfect grid,
/// and compare it with the experimental points.
///
/// @param point_x X coordinates of points.
/// @param point_y Y coordinates of points.
/// @param dest_x X coordinates of final points.
/// @param dest_y Y coordinates of final points.
/// @param width Width of surface.
/// @param height Height of surface.
/// @return nothing.
void get_good_points(struct list* point_x, struct list* point_y, struct list** dest_x, struct list** dest_y, double width, double height)
{
    double hypo_width_cell = width / 9;
    double hypo_height_cell = height / 9;

    double threshold = hypo_width_cell / 3;

    double hypo_grid_x[9];
    double hypo_grid_y[9];

    for (int j = 0; j < 9; j++)
    {
        hypo_grid_x[j] = j * hypo_width_cell;
    }

    for (int i = 0; i < 9; i++)
    {
        hypo_grid_y[i] = i * hypo_height_cell;
    }

    // Now with have a hypothetical grid to compare with the real one with intersections.
    double size = list_len(point_x);
    double *real_grid_x = list_to_array(point_x);
    double *real_grid_y = list_to_array(point_y);

    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            double hypo_x = hypo_grid_x[x];
            double hypo_y = hypo_grid_y[y];

            int i;

            for (i = 0; i < size ; i++)
            {
                double real_x = real_grid_x[i];
                double real_y = real_grid_y[i];

                if (distance(real_x, real_y, hypo_x, hypo_y) < threshold)
                {
                    *dest_x = list_insert_head(*dest_x, real_x);
                    *dest_y = list_insert_head(*dest_y, real_y);
                    break;
                }
            }

            if (i == size)
            {
                *dest_x = list_insert_head(*dest_x, hypo_x);
                *dest_y = list_insert_head(*dest_y, hypo_y);
            }
        }
    }
}

/// @brief Extracts cells and put them in files.
///
/// @param list_x List of x coordinates (81 x coordinates).
/// @param list_y List of y coordinates (81 y coordinates).
/// @param surf Initial surface.
/// @return nothing.
void cell_extraction(struct list* list_x, struct list* list_y, SDL_Surface* surf)
{
    double width = surf->w / 9;
    double height = surf->h / 9;

    double *arr_x = list_to_array(list_x);
    double *arr_y = list_to_array(list_y);

    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            SDL_Rect rect;
            rect.h = height;
            rect.w = width;
            rect.x = arr_x[x + 9 * y];
            rect.y = arr_y[x + 9 * y];

            SDL_Surface* new_surf = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
            SDL_BlitSurface(surf, &rect, new_surf, NULL);

            char filepath[100];
            snprintf(filepath, sizeof(filepath), "img/%i_%i.png", 8 - y, 8 - x);
            IMG_SavePNG(new_surf, filepath);

            SDL_FreeSurface(new_surf);
        }
    }
}

/// @brief Full process to detect the grid and the digits.
///
/// @param list_rho Initial list of rhos from
/// @param list_theta Initial list of thetas.
/// @param surf_sudoku Surface from the image
/// @return nothing.
void grid_detection(struct list* list_rho, struct list* list_theta, SDL_Surface* surf_sudoku)
{
    // Parameters.
    double width = surf_sudoku->w;
    double height = surf_sudoku->h;
    double diag = sqrt(width * width + height * height);

    // AVERAGING LINES.
    // ==========================================
    struct list* list_rho_av = list_new();
    struct list* list_theta_av = list_new();
    average_lines(diag, list_rho, list_theta, &list_rho_av, &list_theta_av);
    // ==========================================

    // GETS ALL INTERSECTIONS OF ALL LINES
    // (INCLUDES ORTHOGONAL FILTER)
    // ==========================================
    struct list* list_point_x = list_new();
    struct list* list_point_y = list_new();
    get_intersections(width, height, diag, list_rho_av, list_theta_av, &list_point_x, &list_point_y);
    // ==========================================


    // KEEPS ONLY GOOD POINTS
    // ==========================================
    struct list* final_grid_x = list_new();
    struct list* final_grid_y = list_new();
    get_good_points(list_point_x, list_point_y, &final_grid_x, &final_grid_y, width, height);
    // ==========================================


    // EXTRACTS CELLS - THE END
    // ==========================================
    cell_extraction(final_grid_x, final_grid_y, surf_sudoku);
    // ==========================================


    // Frees memory.
    list_destroy(list_rho_av);
    list_destroy(list_theta_av);
    list_destroy(list_point_x);
    list_destroy(list_point_y);
    list_destroy(final_grid_x);
    list_destroy(final_grid_y);
}