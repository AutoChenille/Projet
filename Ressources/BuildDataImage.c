#include "BuildDataImage.h"
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <dirent.h>
#include <stddef.h>
#include <unistd.h> 

//Images size
size_t size = 24;

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.

SDL_Surface* load_image2(char* path)
{
    SDL_Surface *tmp = IMG_Load(path);
    SDL_Surface *img = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tmp);
    return img;
}

double pixel_to_grayscale2(Uint32 pixel_color, SDL_PixelFormat* format)
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
        pixels[i] = pixel_to_grayscale2(pixels[i], format);
    }

    SDL_UnlockSurface(surface);
}

matrix *imageToMatrix(char* path)
{
    // - Create a surface from the colored image.
    SDL_Surface *surface = load_image2(path);

    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    matrix *dataImage = Matrix(len, 1);
    
    SDL_LockSurface(surface);
    
    for(int i = 0; i < len; i++)
    {
        dataImage->data[i] = pixel_to_grayscale2(pixels[i], format)/255.;
        dataImage->data[i] = dataImage->data[i] >= 0.3 ? 1 : 0;
        //dataImage->data[i] = pixels[i]/255.;
    }

    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    return dataImage;
}

matrix *surfaceToMatrix(SDL_Surface *surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    //SDL_PixelFormat* format = surface->format;

    matrix *dataImage = Matrix(len, 1);
    
    for(int i = 0; i < len; i++)
        dataImage->data[i] = pixels[i]/255.;

    return dataImage;
}

matrix *LoadFromSurface(SDL_Surface** surface, size_t nbData)
{
    for(size_t i = 0; i < nbData * nbData; i++)
    {
        char filepath[100];
        snprintf(filepath, sizeof(filepath), "img2/%li.png",i);
        IMG_SavePNG(surface[i], filepath);
    }

    nbData = nbData * nbData;
    size_t h = 24, w = 24;
    matrix* loaded = Matrix(h*w, nbData);

    for(size_t j = 0; j < nbData; j++)
    {
        matrix* l_surface = surfaceToMatrix(surface[j]);
        for(size_t i = 0; i < h*w; i++)
            loaded->data[i*nbData+j] = l_surface->data[i];
    }

    return loaded;
}

size_t count_png_files(char *path) {
    size_t count = 0;
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        fprintf(stderr, "Error opening directory %s\n", path);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".png") != NULL) {
            count++;
        } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subdir_path[1024];
            snprintf(subdir_path, sizeof(subdir_path), "%s/%s", path, entry->d_name);
            count += count_png_files(subdir_path);
        }
    }

    closedir(dir);

    return count;
}

datas *get_imgList(char *path, size_t size)
{
    //Get current repo
    char current_repo[1024];

    if (getcwd(current_repo, sizeof(current_repo)) == NULL) {
        // Handle the error condition, e.g., display an error message or take appropriate action
        perror("getcwd failed");
    }

    //total number of png to compute
    size_t nbData = count_png_files(path);

    // directory stream variable for accessing the directory
    DIR *directory;

    // will store pointer to each entry in the directory
    struct dirent *entry;
  
    // attempt to open the directory
    directory = opendir(path);
  
    // if opening the directory fails, exit with an error message and status
    if (directory == NULL)
        errx(EXIT_FAILURE, "Unfound directory %s", path);

    size_t h = size, w = size;
    datas *loaded = malloc(sizeof(datas));
    loaded->input = Matrix(h*w, nbData);
    loaded->output = MatrixOf(size, nbData, 0);

    // Loop through each entry in the directory
    size_t i = 0;

    if (chdir(path) != 0) {
        // Handle the error condition, e.g., display an error message or take appropriate action
        perror("chdir failed");
    }

    while ((entry = readdir(directory)) != NULL)
    {
        // If the entry is a regular file, load the image and add its data to dataList
        if (entry->d_type == DT_REG)
        {
            //printf("%s%s\n", path, entry->d_name);
            matrix *imgData = imageToMatrix(entry->d_name);
            for (size_t j = 0; j < h*w; j++)
            {
                loaded->input->data[j*loaded->input->col+i] = imgData->data[j];
            }
            if(size == 16)
                loaded->output->data[strtol(&entry->d_name[0], NULL, 16)*loaded->output->col+i] = 1;
            else
                loaded->output->data[(entry->d_name[0] - (char)48)*loaded->output->col+i] = 1;
            freeMatrix(imgData);

            i++;
        }
    }

    // Close the directory
    if (closedir(directory) == -1)
        errx(EXIT_FAILURE, "Failed to close directory.");

    //Return in origin repo
    if (chdir(current_repo) != 0) {
        // Handle the error condition, e.g., display an error message or take appropriate action
        perror("chdir failed");
    }

    return loaded;
}
