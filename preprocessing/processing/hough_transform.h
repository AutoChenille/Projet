#ifndef HOUGH_TRANSFORM_H
#define HOUGH_TRANSFORM_H

// Libraries.
// ========================
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// ========================

// Files.
// ========================
#include "doubles_lists.h"
#include "basic_utilities.h"
#include "SDL_utilities.h"
// ========================

// Colors for fun.
// ========================
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_RESET  "\x1b[0m"
// ========================

void hough_transform(SDL_Surface* surf, struct list** list_theta, struct list** list_rho, const double THRESHOLD_PICK_LINES);

#endif