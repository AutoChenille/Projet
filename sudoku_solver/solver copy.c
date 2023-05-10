#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "useful.c"

// Global variable to deal with sudoku and hexadoku.
int HEXA;
size_t GRID_DIMENSION;
size_t BOX_DIMENSION;

// Get grid from file and store it in an array.
//
// filename: path of the file where the grid is stored.
// grid: grid where the sudoku in file is put.
void get_grid_from_file(char filepath[], char grid[GRID_DIMENSION][GRID_DIMENSION])
{
    // Point on the file where the grid is stored.
    FILE *file = fopen(filepath, "r");

    // Throw error if the file is not found.
    if (file == NULL)
        errx(EXIT_FAILURE, "File not found.");
  
    // Store all digits and point in the grid by reading each lines.
    for (size_t z = 0; z < BOX_DIMENSION; z++) 
    {
        // First blocks.
        for (size_t i = z * BOX_DIMENSION; i < BOX_DIMENSION + z * BOX_DIMENSION; i++) 
        {   
            for (size_t j = 0; j < BOX_DIMENSION; j++) 
            {
                // 3 or 4 characters.
                for (size_t k = j * BOX_DIMENSION; k < BOX_DIMENSION + j * BOX_DIMENSION; k++) 
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
void write_grid_in_file(char grid[GRID_DIMENSION][GRID_DIMENSION], char filepath[])
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
    for (size_t z = 0; z < BOX_DIMENSION; z++) 
    {
        for (size_t i = z * BOX_DIMENSION; i < BOX_DIMENSION + z * BOX_DIMENSION; i++) 
        {
            // First blocks (2 for sudoku and 3 for hexadoku).
            for (size_t j = 0; j < BOX_DIMENSION - 1; j++) 
            {
                // 3 charaters for sudoku and 4 for hexadoku.
                for (size_t k = j * BOX_DIMENSION; k < BOX_DIMENSION + j * BOX_DIMENSION; k++) 
                {
                    fprintf(result_file, "%c", grid[i][k]);
                }
        
                // Deal with ' '.
                fprintf(result_file, " ");
            }
        
            // Last characters.
            for (size_t k = (BOX_DIMENSION - 1) * BOX_DIMENSION; k < GRID_DIMENSION; k++) 
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
int number_is_valid(char digit, char grid[GRID_DIMENSION][GRID_DIMENSION], size_t row, size_t col)
{
    // Checks column.
    for (size_t i = 0; i < GRID_DIMENSION; i++) 
    {
        if (grid[i][col] == digit)
            return 0;
    }

    // Checks row.
    for (size_t i = 0; i < GRID_DIMENSION; i++) 
    {
        if (grid[row][i] == digit)
            return 0;
    } 
     
    // Inits row and column of current box.
    size_t begin_row = row - row % BOX_DIMENSION;
    size_t begin_col = col - col % BOX_DIMENSION;

    // Checks box.
    for (size_t i = begin_row; i < begin_row + BOX_DIMENSION; i++) 
    {
        for (size_t j = begin_col; j < begin_col + BOX_DIMENSION; j++) 
        {
            if (grid[i][j] == digit)
                return 0;
        } 
    }
    
    // Returns number is valid in grid.
    return 1;
}

void immediat_solutions(char grid[GRID_DIMENSION][GRID_DIMENSION], char possibilities[GRID_DIMENSION*GRID_DIMENSION][GRID_DIMENSION])
{
    struct Node*** solutions = calloc(GRID_DIMENSION, sizeof(struct Node**));
    for(size_t i = 0; i < GRID_DIMENSION; i++)
    {
        solutions[i] = calloc(GRID_DIMENSION, sizeof(struct Node*));
        for(size_t j = 0; j < GRID_DIMENSION; j++)
        {
            solutions[i][j] = newList();
            if(grid[i][j] == '.')
            {
                for(char n = (HEXA?0:1); n < (char)GRID_DIMENSION+(HEXA?0:1); n++)
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
        for(size_t x = 0; x < GRID_DIMENSION; x++)
        {
            for(size_t y = 0; y < GRID_DIMENSION; y++)
            {
                if(grid[x][y] == '.')
                {
                    //Check column
                    for(size_t i = 0; i < GRID_DIMENSION; i++)
                    {
                        if(grid[i][y] != '.')
                            gotchanges = max(gotchanges, remove_list(solutions[x][y], grid[i][y]));
                    }
                    //Check lines
                    for(size_t j = 0; j < GRID_DIMENSION; j++)
                    {
                        if(grid[x][j] != '.')
                            gotchanges = max(gotchanges, remove_list(solutions[x][y], grid[x][j]));
                    }
                    //Check box
                    for(size_t i = x/BOX_DIMENSION * BOX_DIMENSION; i < (x/BOX_DIMENSION + 1) * BOX_DIMENSION; i++)
                    {
                        for(size_t j = y/BOX_DIMENSION * BOX_DIMENSION; j < (y/BOX_DIMENSION + 1) * BOX_DIMENSION; j++)
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

    for(size_t i = 0; i < GRID_DIMENSION; i++)
    {
        for(size_t j = 0; j < GRID_DIMENSION; j++)
        {
            size_t pos = i*GRID_DIMENSION + j;
            size_t k = 0;
            struct Node* current = solutions[i][j]->next;
            while(current != NULL)
            {
                possibilities[pos][k] = current->data;
                current = current->next;
                k++;
            }
            if(k < GRID_DIMENSION)
                possibilities[pos][k] = -1;
        }
    }
}

// Solves sudoku.
//
// grid: grid where the digits of sudoku.
// pos: current pos the function is dealing with.
int solve(char grid[GRID_DIMENSION][GRID_DIMENSION], char possibilities[GRID_DIMENSION*GRID_DIMENSION][GRID_DIMENSION], size_t pos)
{
    // Stops if it is the last coords.
    if (pos == GRID_DIMENSION * GRID_DIMENSION)
        return 1;

    // Gets row and col from the current position.
    size_t row = pos / GRID_DIMENSION;
    size_t col = pos % GRID_DIMENSION;

    // Got to the next cell if there is already a number in the cell (row, col).
    if (grid[row][col] != '.')
        return solve(grid, possibilities, pos + 1);
    
    // Tests numbers between 1 and 9 for the cell (row, col).
    for (size_t i = (HEXA?0:1); i < GRID_DIMENSION+(HEXA?0:1) && possibilities[pos][i] != -1; i++)
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


void draw_image(char grid[GRID_DIMENSION][GRID_DIMENSION], char filepath[])
{
    // Load the blank grid image
    SDL_Surface* grid_surface = IMG_Load("blank-grid.png");

    // Load the digit images
    SDL_Surface* digit_surfaces[GRID_DIMENSION];
    for (size_t i = (HEXA?0:1); i <= GRID_DIMENSION; i++) {
        char filename[10];
        sprintf(filename, "%li.png", i);
        digit_surfaces[i-1] = IMG_Load(filename);
    }

    // Create a surface to draw on
    SDL_Surface* draw_surface = SDL_CreateRGBSurface(0, grid_surface->w, grid_surface->h, 32, 0, 0, 0, 0);

    // Iterate through the grid and draw the digits
    for (size_t row = 0; row < GRID_DIMENSION; row++) {
        for (size_t col = 0; col < GRID_DIMENSION; col++) {
            // Calculate the position of the cell in the image
            int x = col * digit_surfaces[0]->w;
            int y = row * digit_surfaces[0]->h;

            // Get the digit to draw
            int digit = grid[row][col] - '0';

            // Blit the digit image onto the surface
            SDL_Rect src_rect = {15, 15, digit_surfaces[digit-1]->w - 30, digit_surfaces[digit-1]->h - 30};
            SDL_Rect dest_rect = {x + 10, y + 10, digit_surfaces[digit-1]->w - 20, digit_surfaces[digit-1]->h - 20};
            SDL_BlitSurface(digit_surfaces[digit-1], &src_rect, grid_surface, &dest_rect);
        }
    }

    // Save the surface as a PNG image
    char result_filepath[strlen(filepath) + 12];
    sprintf(result_filepath, "%s-result.png", filepath);
    IMG_SavePNG(grid_surface, result_filepath);

    // Clean up
    SDL_FreeSurface(grid_surface);
    for (size_t i = 0; i < GRID_DIMENSION; i++)
    {
        SDL_FreeSurface(digit_surfaces[i]);
    }
    SDL_FreeSurface(draw_surface);
}

// Main function.
int main(int argc, char *argv[])
{
    // Exit if there is not only one parameter.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: filepath + hexa");

    HEXA = (int) (*argv[2] - '0');
    GRID_DIMENSION = HEXA ? 16 : 9;
    BOX_DIMENSION = HEXA ? 4 : 3;
    
    // Creates a new empty grid.
    char grid[GRID_DIMENSION][GRID_DIMENSION];
    char possibilities[GRID_DIMENSION*GRID_DIMENSION][GRID_DIMENSION];

    // Get the grid from file in one array.
    get_grid_from_file(argv[1], grid);

    immediat_solutions(grid, possibilities);

    // Solves sudoku by checking if the original grid is not valid.
    if (!solve(grid, possibilities, 0))
        errx(EXIT_FAILURE, "File found, but grid not valid.");

    // Saves the result in a new file.
    write_grid_in_file(grid, argv[1]);

    //draw_image(grid, argv[1]);

    // Exits program with success.
    return EXIT_SUCCESS;
}
