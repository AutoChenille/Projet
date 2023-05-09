#pragma once
#include <stdio.h>
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//Rotation
SDL_Surface *rotate(SDL_Surface *surface, double degree);

//Perspective Transform
SDL_Surface* perspective_correction(SDL_Surface *input, double corners[4][2]);

//Filters
void grayscale(SDL_Surface *surface);
void sobel_filter(SDL_Surface *surface);
void blur(SDL_Surface *surface, int strength);
double detect_angle(int corners[4][2]);
void gaussian_blur(SDL_Surface *surface, int radius, double sigma);
int str_to_int(char *str);
void median_filter(SDL_Surface *surface, int window_size);
SDL_Surface* dilate(SDL_Surface* input, int kernel_size);
SDL_Surface* invert(SDL_Surface *input);
void sauvola(SDL_Surface *surface, int window_size, double k);
void enhance_contrast(SDL_Surface *surface);
SDL_Surface* threshold(SDL_Surface *input, Uint8 threshold_value);
void otsu_adaptive_threshold(SDL_Surface *surface);
SDL_Surface* adaptive_threshold(SDL_Surface* surface, int neighborhood_size, int C);
