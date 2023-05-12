#pragma once
#include <limits.h>
#include <stdio.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Perspective Transform
SDL_Surface* perspective_transform(SDL_Surface* surface, double corners[4][2]);

// Edge Detection
void canny(SDL_Surface* grayscaleImage);

// Filters
void enhance_contrast(SDL_Surface *surface);
void grayscale(SDL_Surface *surface);
void gaussian_blur(SDL_Surface *surface, int radius, double sigma);
double noise_level(SDL_Surface* surface);
void adaptive_threshold(SDL_Surface* surface, double threshold);
void canny(SDL_Surface* grayscaleImage);
void dilate(SDL_Surface* input);
SDL_Surface* invert(SDL_Surface *input);
SDL_Surface* threshold(SDL_Surface *input, Uint8 threshold_value);

// Utils
int str_to_int(char *str);
