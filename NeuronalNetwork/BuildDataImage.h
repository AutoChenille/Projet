#include <SDL2/SDL_surface.h>
#include <err.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include "matrix.h"

typedef struct 
{
    matrix *input;
    matrix *output;

}datas;

SDL_Surface* load_image(const char* path);

datas *get_imgList(const char* dirpath);

matrix *imageToMatrix(const char* path);