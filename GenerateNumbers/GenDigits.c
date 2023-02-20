#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "GenDigits.h"

void generate_images() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *window = SDL_CreateWindow("Canvas with digits", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 30, 30, 0);
    if (!window) {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "Unable to create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface *surface;
    SDL_Texture *texture;

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("arial.ttf", 10);

    char digits[10][2] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    for (int i = 0; i < 10; i++) {
        surface = TTF_RenderText_Solid(font, digits[i], color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect rect;
        rect.x = 1 + 3 * i;
        rect.y = 1;
        rect.w = surface->w;
        rect.h = surface->h;

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Generate file name
        char file_name[20];
        snprintf(file_name, 20, "digit_%d.bmp", i);

        // Save the canvas as a bitmap file
        SDL_Surface *canvas = SDL_GetWindowSurface(window);
        SDL_SaveBMP(canvas, file_name);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_RenderPresent(renderer);

    SDL_Delay(5000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
