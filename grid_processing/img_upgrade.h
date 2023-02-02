#ifndef IMG_UPGRADE_H
#define IMG_UPGRADE_H

// Libraries.
// ==============
#include <math.h>
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


int fill_left(int width, int height, int* pixels);
int fill_right(int width, int height, int* pixels);
int fill_top(int width, int height, int* pixels);
int fill_bottom(int width, int height, int* pixels);
void upgrade_exploitation(int debug, SDL_Surface* surf, SDL_Surface** dest);

#endif