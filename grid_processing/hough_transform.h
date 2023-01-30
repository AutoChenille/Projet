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

void hough_transform(SDL_Surface* surf, int debug, struct list** list_theta, struct list** list_rho);

#endif