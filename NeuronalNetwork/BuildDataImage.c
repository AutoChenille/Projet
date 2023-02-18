#include "BuildDataImage.h"
#include <SDL2/SDL_surface.h>
#include <stddef.h>
#include <stdlib.h>

#include<unistd.h> 

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface *tmp = IMG_Load(path);
    SDL_Surface *img = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    free(tmp);
    return img;
}

double pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    double average = 0.3*r + 0.59*g + 0.11*b;
    //return SDL_MapRGB(format, average, average, average);
    return average;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    
    SDL_LockSurface(surface);
    
    for(int i = 0; i < len; i++)
    {
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    }

    SDL_UnlockSurface(surface);
}

matrix *imageToMatrix(const char* path)
{
    // - Create a surface from the colored image.
    SDL_Surface *surface = load_image(path);

    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    matrix *dataImage = Matrix(1, len);
    
    SDL_LockSurface(surface);
    
    for(int i = 0; i < len; i++)
    {
        dataImage->data[i] = pixel_to_grayscale(pixels[i], format);
    }

    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    return dataImage;
}

matrix *get_imgList(const char* dirpath)
{
    // directory stream variable for accessing the directory
    DIR *directory;

    // will store pointer to each entry in the directory
    struct dirent *entry;
  
    // attempt to open the current working directory, opendir() returns NULL 
    // on failure
    directory = opendir(dirpath);
  
    // if opening the directory fails, exit with an error message and status
    if (directory == NULL)
        errx(EXIT_FAILURE, "Unfound directory %s", dirpath);

    
    chdir(dirpath);

    size_t h = 92, w = 92;
    matrix *dataList = Matrix(3235, h*w);
  
    // Read each entry in the directory with readdir() and store the pointer to 
    // the struct dirent into entry... when there are no more entries in the 
    // directory readdir() will return NULL and terminate the loop at that point.
    size_t i = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        // If the entry is a regular file, output the filename prepended with 
        // "File: " so we know it is a file when examining the program output
        if (entry->d_type == DT_REG)
        {
            printf("open %s\n", entry->d_name);
            for(size_t j = 0; j < h*w; j++)
            {
                matrix *imgData = imageToMatrix(entry->d_name);
                dataList->data[i*dataList->col+j] = imgData->data[j];
                m_free(imgData);
            }
            i ++;
        }

        // Otherwise if the entry is a directory, output the directory name 
        // prepended with "dir: " again so we know what what it is when looking 
        // at the program output.
        //else if (entry->d_type == DT_DIR)
        //    printf(" dir: %s\n", entry->d_name);
    }
  
    // close the directory... if closedir() fails it will return -1
    if (closedir(directory) == -1)
        errx(EXIT_FAILURE, "Failed to close directory.");
  
    return dataList;
}