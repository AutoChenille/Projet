#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

// Files.
// =====
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// =====

#include <string.h>

void orthogonal_filter(struct list* list_rho, struct list* list_theta,
                       struct list** list_rho_vert, struct list** list_theta_vert,
                       struct list** list_rho_hori, struct list** list_theta_hori);

void average_lines(double diag, struct list* list_rho, struct list* list_theta, struct list** dest_rho, struct list** dest_theta);
void point_intersection(double surf_diag, double rho1, double theta1, double rho2, double theta2, double* x, double* y);
void get_intersections(double w, double h, double diag, struct list* list_rho, struct list* list_theta, struct list** list_point_x, struct list** list_point_y);
void get_extremes_lines(SDL_Surface* surf, double diag, struct list* list_rho, struct list* list_theta);
void average_points(struct list* list_x,  struct list* list_y, struct list** dest_x,  struct list** dest_y);
double distance_horizontal_lines(double rho1, double theta1, double rho2, double theta2, double width);
void grid_detection(int debug, struct list* list_rho, struct list* theta, double diag, SDL_Surface* surf);

#endif