#ifndef SDL_UTILITIES_H
#define SDL_UTILITIES_H

// Libraries
// ========================
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// ========================

// Files.
// =========================
#include "basic_utilities.h"
#include "doubles_lists.h"
// =========================

SDL_Surface* load_image(const char path[]);
SDL_Surface* array_to_surface(int w, int h, int accumulator[]);
int pixel_to_bit_value(SDL_Surface* surf, Uint32 color);
void get_array_of_pixels(SDL_Surface* surf, int pixel_bit_array[]);
void event_loop();
void show_surface_on_window(SDL_Surface* surf);
void draw_lines_on_window(struct list* list_rho, struct list* list_theta, char *path, double surf_diag);

#endif