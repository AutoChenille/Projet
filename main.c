#include <SDL2/SDL.h>
#include <stddef.h>

//Preprocessing
#include "Preprocessing/pretreatment.h"

//Processing
#include "Processing/main.c"
#include "NeuronalNetwork/mainNN.h"
#include "sudoku_solver/solver9.h"


int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1; // Quitter l'application avec un code d'erreur
    }

    char paf[] = "./Processing/pimg/image_02.jpeg";
    int NB_CELLS = 9;

    SDL_Surface **loaded = ProcessImage(paf, NB_CELLS);

    printf("les wokes sont pas droles\n");

    for (int i = 0; i < 81; i++)
    {
        char filepath[100];
        snprintf(filepath, sizeof(filepath), "img2/%i.png", i);
        IMG_SavePNG(loaded[i], filepath);
    }

    printf("all saved\n");
    
    if(1)
    {
        char** tosolve = PredictSurface_9x9(loaded, NB_CELLS,"./NeuronalNetwork/800x3_with_blank_handwrite");

        /*
        for(size_t i = 0; i < NB_CELLS; i++) {
            for(size_t j = 0; j < NB_CELLS; j++) {
                printf("%c ", tosolve[i][j]);
            }
            printf("\n");
        }
        */

	    char tosolve9[9][9];
	    for (size_t i = 0; i < 9; i++)
	        for (size_t j = 0; j < 9; j++)
	            tosolve9[i][j] = tosolve[i][j];

        if(tosolve == NULL)
            printf("Error while loading grid");

        printf("unsolved 9x9\n");
        draw_sudoku(tosolve9, "./unsolved.png");
        printf("unsolved 9x9 done\n");
    }

    SDL_Quit();

    return 0;

}
