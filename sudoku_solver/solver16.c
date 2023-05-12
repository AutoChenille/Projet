#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "solver16.h"

// Global variable to deal with sudoku and hexadoku.
int HEXA16 = 1;
size_t GRID_DIMENSION16 = 16;
size_t BOXDIMENSION16 = 4;


// Get grid from file and store it in an array.
//
// filename: path of the file where the grid is stored.
// grid: grid where the sudoku in file is put.
void get_grid_from_file16(char filepath[], char grid[GRID_DIMENSION16][GRID_DIMENSION16])
{
    // Point on the file where the grid is stored.
    FILE *file = fopen(filepath, "r");

    // Throw error if the file is not found.
    if (file == NULL)
        errx(EXIT_FAILURE, "File not found.");
  
    // Store all digits and point in the grid by reading each lines.
    for (size_t z = 0; z < BOXDIMENSION16; z++) 
    {
        // First blocks.
        for (size_t i = z * BOXDIMENSION16; i < BOXDIMENSION16 + z * BOXDIMENSION16; i++) 
        {   
            for (size_t j = 0; j < BOXDIMENSION16; j++) 
            {
                // 3 or 4 characters.
                for (size_t k = j * BOXDIMENSION16; k < BOXDIMENSION16 + j * BOXDIMENSION16; k++) 
                {
                    grid[i][k] = fgetc(file);
                }
        
                // Deals with ' ' and '\n'.
                fgetc(file);
            }
        }
        
        // Deals with '\n'.
        fgetc(file);
    }

    // Closes file.
    fclose(file);
}

// Save the grid in a new file with extension ".result".
//
// grid: grid with the digits of sudoku.
// filepath: path of the file where the grid is stored.
void write_grid_in_file(char grid[GRID_DIMENSION16][GRID_DIMENSION16], char filepath[])
{
    // Concate filepath and extension.
    char filepath_extension[strlen(filepath) + 7];
    filepath_extension[0] = '\0';
    strcat(filepath_extension, filepath);
    strcat(filepath_extension, ".result");
    
    // Creates a new file.
    FILE *result_file = fopen(filepath_extension, "w");

    // Check if file has been created.
    if (result_file == NULL)
        errx(EXIT_FAILURE, "Unable to create file.");

    // Writes in file.
    for (size_t z = 0; z < BOXDIMENSION16; z++) 
    {
        for (size_t i = z * BOXDIMENSION16; i < BOXDIMENSION16 + z * BOXDIMENSION16; i++) 
        {
            // First blocks (2 for sudoku and 3 for hexadoku).
            for (size_t j = 0; j < BOXDIMENSION16 - 1; j++) 
            {
                // 3 charaters for sudoku and 4 for hexadoku.
                for (size_t k = j * BOXDIMENSION16; k < BOXDIMENSION16 + j * BOXDIMENSION16; k++) 
                {
                    fprintf(result_file, "%c", grid[i][k]);
                }
        
                // Deal with ' '.
                fprintf(result_file, " ");
            }
        
            // Last characters.
            for (size_t k = (BOXDIMENSION16 - 1) * BOXDIMENSION16; k < GRID_DIMENSION16; k++) 
            {
                fprintf(result_file, "%c", grid[i][k]);
            }

            // Writes a new line between blocks.
            fprintf(result_file, "\n");
        }
        
        // Writes a new line betwenn blocks.
        fprintf(result_file, "\n");
    } 
      
    // Closes the new file.
    fclose(result_file);
}

// Check if a number can be put in the grid.
//
// digit: digit to test in grid.
// grid: grid with the digits of sudoku.
// row: row of the digit in grid.
// col: column of the digit in grid.
int number_is_valid(char digit, char grid[GRID_DIMENSION16][GRID_DIMENSION16], size_t row, size_t col)
{
    // Checks column.
    for (size_t i = 0; i < GRID_DIMENSION16; i++) 
    {
        if (grid[i][col] == digit)
            return 0;
    }

    // Checks row.
    for (size_t i = 0; i < GRID_DIMENSION16; i++) 
    {
        if (grid[row][i] == digit)
            return 0;
    } 
     
    // Inits row and column of current box.
    size_t begin_row = row - row % BOXDIMENSION16;
    size_t begin_col = col - col % BOXDIMENSION16;

    // Checks box.
    for (size_t i = begin_row; i < begin_row + BOXDIMENSION16; i++) 
    {
        for (size_t j = begin_col; j < begin_col + BOXDIMENSION16; j++) 
        {
            if (grid[i][j] == digit)
                return 0;
        } 
    }
    
    // Returns number is valid in grid.
    return 1;
}

void immediat_solutions(char grid[GRID_DIMENSION16][GRID_DIMENSION16], char possibilities[GRID_DIMENSION16*GRID_DIMENSION16][GRID_DIMENSION16])
{
    struct Node*** solutions = calloc(GRID_DIMENSION16, sizeof(struct Node**));
    for(size_t i = 0; i < GRID_DIMENSION16; i++)
    {
        solutions[i] = calloc(GRID_DIMENSION16, sizeof(struct Node*));
        for(size_t j = 0; j < GRID_DIMENSION16; j++)
        {
            solutions[i][j] = newList();
            if(grid[i][j] == '.')
            {
                for(char n = (HEXA16?0:1); n < (char)GRID_DIMENSION16+(HEXA16?0:1); n++)
                    insert_list(solutions[i][j], (n > 9 ? n+7: n)+'0');
            }
            else
                insert_list(solutions[i][j], grid[i][j]);
        }
    }

    int gotchanges = 1;
    while(gotchanges)
    {
        gotchanges = 0;
        for(size_t x = 0; x < GRID_DIMENSION16; x++)
        {
            for(size_t y = 0; y < GRID_DIMENSION16; y++)
            {
                if(grid[x][y] == '.')
                {
                    //Check column
                    for(size_t i = 0; i < GRID_DIMENSION16; i++)
                    {
                        if(grid[i][y] != '.')
                            gotchanges = max(gotchanges, remove_list(solutions[x][y], grid[i][y]));
                    }
                    //Check lines
                    for(size_t j = 0; j < GRID_DIMENSION16; j++)
                    {
                        if(grid[x][j] != '.')
                            gotchanges = max(gotchanges, remove_list(solutions[x][y], grid[x][j]));
                    }
                    //Check box
                    for(size_t i = x/BOXDIMENSION16 * BOXDIMENSION16; i < (x/BOXDIMENSION16 + 1) * BOXDIMENSION16; i++)
                    {
                        for(size_t j = y/BOXDIMENSION16 * BOXDIMENSION16; j < (y/BOXDIMENSION16 + 1) * BOXDIMENSION16; j++)
                        {
                            if(grid[i][j] != '.')
                                gotchanges = max(gotchanges, remove_list(solutions[x][y], grid[i][j]));
                        }
                    }

                    if(solutions[x][y]->next->next == NULL)
                        grid[x][y] = solutions[x][y]->next->data;
                }
            }
        }
    }

    for(size_t i = 0; i < GRID_DIMENSION16; i++)
    {
        for(size_t j = 0; j < GRID_DIMENSION16; j++)
        {
            size_t pos = i*GRID_DIMENSION16 + j;
            size_t k = 0;
            struct Node* current = solutions[i][j]->next;
            while(current != NULL)
            {
                possibilities[pos][k] = current->data;
                current = current->next;
                k++;
            }
            if(k < GRID_DIMENSION16)
                possibilities[pos][k] = -1;
        }
    }
}

// Solves sudoku.
//
// grid: grid where the digits of sudoku.
// pos: current pos the function is dealing with.
int solve(char grid[GRID_DIMENSION16][GRID_DIMENSION16], char possibilities[GRID_DIMENSION16*GRID_DIMENSION16][GRID_DIMENSION16], size_t pos)
{
    // Stops if it is the last coords.
    if (pos == GRID_DIMENSION16 * GRID_DIMENSION16)
        return 1;

    // Gets row and col from the current position.
    size_t row = pos / GRID_DIMENSION16;
    size_t col = pos % GRID_DIMENSION16;

    // Got to the next cell if there is already a number in the cell (row, col).
    if (grid[row][col] != '.')
        return solve(grid, possibilities, pos + 1);
    
    // Tests numbers between 1 and 9 for the cell (row, col).
    for (size_t i = 0; i < GRID_DIMENSION16 && possibilities[pos][i] != -1; i++)
    {
        char possible_nb = possibilities[pos][i];
        if (number_is_valid(possible_nb, grid, row, col))
        {
            // Sets the cell (row, col) with the possible number.
            grid[row][col] = possible_nb;
            
            // Checks if the next cell won't have any problem in the grid.
            // Otherwise, continues by setting current cell with the next number.
            if (solve(grid, possibilities, pos + 1))
                return 1;
        }
    }

    // Reinitializes grid to '.' if any number is possible.
    grid[row][col] = '.';

    // The loop exits, so there is not a possible number to set the cell.
    // One possibility is that the original grid is not valid.
    // The other possibility is there is one cell that is not valid in the grid.
    return 0;
}

void drawHLine(SDL_Surface* surface, int x, int y, int length, Uint32 color, int thickness)
{
    SDL_LockSurface(surface);

    for (int i = 0; i < thickness; i++)
    {
        for (int j = 0; j < length; j++)
        {
            Uint32* pixel = (Uint32*)surface->pixels + (y + i) * surface->pitch / sizeof(Uint32) + (x + j);
            *pixel = color;
        }
    }

    SDL_UnlockSurface(surface);
}

void drawVLine(SDL_Surface* surface, int x, int y, int length, Uint32 color, int thickness)
{
    SDL_LockSurface(surface);

    for (int i = 0; i < thickness; i++)
    {
        for (int j = 0; j < length; j++)
        {
            Uint32* pixel = (Uint32*)surface->pixels + (y + j) * surface->pitch / sizeof(Uint32) + (x + i);
            *pixel = color;
        }
    }

    SDL_UnlockSurface(surface);
}

void draw_hexadoku(char grid[16][16], char filepath[])
{
    size_t CELL_SIZE = 90;

    // Load the blank grid image
    SDL_Surface* grid_surface = SDL_CreateRGBSurface(0, 1440, 1440, 32, 0, 0, 0, 0);
    SDL_FillRect(grid_surface, NULL, SDL_MapRGB(grid_surface->format, 255, 255, 255)); // Fond blanc


    // Load the digit images
    SDL_Surface* digit_surfaces[GRID_DIMENSION16];
    for (size_t i = 0; i < 16; i++) {
        char filename[10];
        sprintf(filename, "%li.png", i);
        digit_surfaces[i] = IMG_Load(filename);
    }

    // Create a surface to draw on
    SDL_Surface* draw_surface = SDL_CreateRGBSurface(0, grid_surface->w, grid_surface->h, 32, 0, 0, 0, 0);

    SDL_FillRect(draw_surface, NULL, SDL_MapRGB(draw_surface->format, 255, 255, 255)); // Fond blanc

    for (int col = 0; col < 16; col++)
        drawVLine(grid_surface, col*CELL_SIZE, 0, 1440, SDL_MapRGB(draw_surface->format, 0, 0, 0), col%4==0?6:1);
    for (int row = 0; row < 16; row++)
        drawHLine(grid_surface, 0, row*CELL_SIZE, 1440, SDL_MapRGB(draw_surface->format, 0, 0, 0), row%4==0?6:1);

    // Iterate through the grid and draw the digits
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
            // Calculate the position of the cell in the image
            int x = col * digit_surfaces[0]->w;
            int y = row * digit_surfaces[0]->h;

            // Get the digit to draw
            int digit;
            if(grid[row][col] <= '9')
                digit = grid[row][col] - '0';
            else if(grid[row][col] == 'A')
                digit = 10;
            else if(grid[row][col] == 'B')
                digit = 11;
            else if(grid[row][col] == 'C')
                digit = 12;
            else if(grid[row][col] == 'D')
                digit = 13;
            else if(grid[row][col] == 'E')
                digit = 14;
            else
                digit = 15;


            // Blit the digit image onto the surface
            SDL_Rect src_rect = {15, 15, digit_surfaces[digit]->w - 30, digit_surfaces[digit-1]->h - 30};
            SDL_Rect dest_rect = {x + 10, y + 10, digit_surfaces[digit]->w - 20, digit_surfaces[digit]->h - 20};
            SDL_BlitSurface(digit_surfaces[digit], &src_rect, grid_surface, &dest_rect);
        }
    }

    // Save the surface as a PNG image
    char result_filepath[strlen(filepath) + 12];
    sprintf(result_filepath, "%s", filepath);
    IMG_SavePNG(grid_surface, result_filepath);

    // Clean up
    SDL_FreeSurface(grid_surface);
    for (int i = 0; i < 9; i++)
    {
        SDL_FreeSurface(digit_surfaces[i]);
    }
    SDL_FreeSurface(draw_surface);
}

// Main function.
int SolveHexadoku(char grid[16][16])
{

    char possibilities[16*16][16];
    immediat_solutions(grid, possibilities);

    // Solves sudoku by checking if the original grid is not valid.
    if (!solve(grid, possibilities, 0))
        errx(EXIT_FAILURE, "File found, but grid not valid.");

    // Exits program with success.
    return EXIT_SUCCESS;
}
