#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

// Files.
// =====
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// =====

#include <string.h>
#include <stdlib.h>

void orthogonal_filter(struct list* list_rho, struct list* list_theta,
                       struct list** list_rho_vert, struct list** list_theta_vert,
                       struct list** list_rho_hori, struct list** list_theta_hori);
void average_lines(double diag, struct list* list_rho,
        struct list* list_theta, struct list** dest_rho, struct list** dest_theta);
void point_intersection(double surf_diag,
                        double rho1,double theta1, double rho2, double theta2, double* x, double* y);
void get_intersections(double w, double h, double diag, struct list* list_rho,
        struct list* list_theta, struct list** list_point_x, struct list** list_point_y);
void get_good_points(struct list* point_x, struct list* point_y,
        struct list** dest_x, struct list** dest_y, double width, double height);
void cell_extraction(struct list* list_x, struct list* list_y, SDL_Surface* surf);
void grid_detection(struct list* list_rho, struct list* list_theta, SDL_Surface* surf_sudoku);

#endif