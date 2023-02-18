#ifndef CONTOUR_MANAGER_H
#define CONTOUR_MANAGER_H

// Libraries.
// ==============
#include <math.h>
#include <stdlib.h>
// ==============

// Files.
// =========================
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// =========================

// Colors for fun.
// ========================
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_RESET  "\x1b[0m"
// ========================

void convex_hull(struct list* point_x, struct list* point_y, struct list** dest_x, struct list** dest_y);
double get_polygon_area(struct list* point_x, struct list* point_y);
void get_points_max_shape(int width, int height, int *pixels, struct list** save_point_x, struct list** save_point_y);
void find_corners_of_rectangle(struct list* point_x, struct list* point_y, double *dest_x, double *dest_y);

// tmp
void TMP_extremes_lines_and_cell_extraction(SDL_Surface* surf, double corners_x[], double corners_y[]);

void get_max_points_rect(SDL_Surface* surf, double *corners_x, double *corners_y);


#endif