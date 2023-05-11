#include <SDL2/SDL_surface.h>
#include "matrix.h"

typedef struct 
{
    matrix *input;
    matrix *output;

}datas;

SDL_Surface* load_image(char* path);

datas *get_imgList(char* dirpath);

matrix *imageToMatrix(char* path);