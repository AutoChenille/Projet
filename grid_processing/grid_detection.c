#include "grid_detection.h"

// Constants
// =========
// Theta threshold in degrees to keep vertical and horizontal lines.
const double THETA_THRESHOLD = 8;
// Rho and theta thresholds in pixels to merge equivalent lines.
const double RHO_THR_AV = 10;
const double THETA_THR_AV = 10;

// Distance maximal to say two lines can be merged.
const double LIMIT_DISTANCE = 64;
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

//
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
        if (arr_rho[index_current_line] == 0 && arr_theta[index_current_line] == -100)
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

                if (close_1 && close_2)
                {
                    arr_rho[index_current_line] = (arr_rho[index_current_line] + arr_rho[index_line]) / 2;
                    arr_theta[index_current_line] = (arr_theta[index_current_line] + arr_theta[index_line]) / 2;

                    // Sets that the line has been treated
                    arr_rho[index_line] = 0;
                    arr_theta[index_line] = -100;
                }
            }
        }
    }

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
void grid_detection(int debug, struct list* list_rho, struct list* list_theta, double diag, SDL_Surface* surf)
{
    // KEEPS ONLY VERTICAL AND HORIZONTAL LINES
    // ========================================
    // Inits lists to separate vertical and horizontal lines.
    struct list* list_rho_vert = list_new();
    struct list* list_theta_vert = list_new();
    struct list* list_rho_hori = list_new();
    struct list* list_theta_hori = list_new();

    // Keeps only vertical and horizontal lines. Erase the other lines.
    // We suppose rotation of the grid has been already performed.
    orthogonal_filter(list_rho, list_theta, &list_rho_vert, &list_theta_vert, &list_rho_hori, &list_theta_hori);

    // Frees lists rho and theta because we don't need them anymore.
    list_destroy(list_rho);
    list_destroy(list_theta);
    // ========================================

    // AVERAGING OF VERTICAL AND HORIZONTAL LINES
    // ==========================================
    // Inits lists to store average vertical lines and gets averaged lines.
    struct list* list_rho_vert_av = list_new();
    struct list* list_theta_vert_av = list_new();
    average_lines(diag, list_rho_vert, list_theta_vert, &list_rho_vert_av, &list_theta_vert_av);

    // Inits lists to store average horizontal lines and gets averaged lines.
    struct list* list_rho_hori_av = list_new();
    struct list* list_theta_hori_av = list_new();
    average_lines(diag, list_rho_hori, list_theta_hori, &list_rho_hori_av, &list_theta_hori_av);
    // ==========================================

    if (debug)
    {
        printf("Vertical: %i -> %i\n", list_len(list_rho_vert), list_len(list_rho_vert_av));
        printf("Horizontal: %i -> %i\n", list_len(list_rho_hori), list_len(list_rho_hori_av));

        draw_lines_on_window(list_rho_vert_av, list_theta_vert_av, surf, diag);
        draw_lines_on_window(list_rho_hori_av, list_theta_hori_av, surf, diag);
    }

    // Frees memory.
    list_destroy(list_rho_vert);
    list_destroy(list_theta_vert);
    list_destroy(list_rho_hori);
    list_destroy(list_theta_hori);
    list_destroy(list_rho_vert_av);
    list_destroy(list_theta_vert_av);
    list_destroy(list_rho_hori_av);
    list_destroy(list_theta_hori_av);
}