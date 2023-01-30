#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

// Files.
// =====
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// =====

void orthogonal_filter(struct list* list_rho, struct list* list_theta,
                       struct list** list_rho_vert, struct list** list_theta_vert,
                       struct list** list_rho_hori, struct list** list_theta_hori);

void average_lines(double diag, struct list* list_rho, struct list* list_theta, struct list** dest_rho, struct list** dest_theta);
void grid_detection(int debug, struct list* list_rho, struct list* theta, double diag, SDL_Surface* surf);

#endif