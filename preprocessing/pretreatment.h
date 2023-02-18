#pragma once

void display(char *filepath);
SDL_Surface *rotate(SDL_Surface *surface, double degree);
void surface_to_grayscale(SDL_Surface *surface);
void sobel_filter(SDL_Surface *surface);
void blur(SDL_Surface *surface, int strength);
