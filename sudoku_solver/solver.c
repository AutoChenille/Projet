#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>

// Global variable to deal with sudoku and hexadoku.
const size_t GRID_DIMENSION = 9;
const size_t BOX_DIMENSION = 3;

// Get grid from file and store it in an array.
//
// filename: path of the file where the grid is stored.
// grid: grid where the sudoku in file is put.
void get_grid_from_file(char filepath[], char grid[9][9])
{
    // Point on the file where the grid is stored.
    FILE *file = fopen(filepath, "r");

    // Throw error if the file is not found.
    if (file == NULL)
        errx(EXIT_FAILURE, "File not found.");
  
    // Store all digits and point in the grid by reading each lines.
    for (size_t z = 0; z < 3; z++) 
    {
        // First blocks.
        for (size_t i = z * 3; i < 3 + z * 3; i++) 
        {   
            for (size_t j = 0; j < 3; j++) 
            {
                // 3 or 4 characters.
                for (size_t k = j * 3; k < 3 + j * 3; k++) 
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
void write_grid_in_file(char grid[9][9], char filepath[])
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
    for (size_t z = 0; z < 3; z++) 
    {
        for (size_t i = z * 3; i < 3 + z * 3; i++) 
        {
            // First blocks (2 for sudoku and 3 for hexadoku).
            for (size_t j = 0; j < 3 - 1; j++) 
            {
                // 3 charaters for sudoku and 4 for hexadoku.
                for (size_t k = j * 3; k < 3 + j * 3; k++) 
                {
                    fprintf(result_file, "%c", grid[i][k]);
                }
        
                // Deal with ' '.
                fprintf(result_file, " ");
            }
        
            // Last characters.
            for (size_t k = (3 - 1) * 3; k < 9; k++) 
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
int number_is_valid(char digit, char grid[9][9], size_t row, size_t col)
{
    // Checks column.
    for (size_t i = 0; i < 9; i++) 
    {
        if (grid[i][col] == digit)
            return 0;
    }

    // Checks row.
    for (size_t i = 0; i < 9; i++) 
    {
        if (grid[row][i] == digit)
            return 0;
    } 
     
    // Inits row and column of current box.
    size_t begin_row = row - row % 3;
    size_t begin_col = col - col % 3;

    // Checks box.
    for (size_t i = begin_row; i < begin_row + 3; i++) 
    {
        for (size_t j = begin_col; j < begin_col + 3; j++) 
        {
            if (grid[i][j] == digit)
                return 0;
        } 
    }
    
    // Returns number is valid in grid.
    return 1;
}

// Solves sudoku.
//
// grid: grid where the digits of sudoku.
// pos: current pos the function is dealing with.
int solve(char grid[9][9], size_t pos)
{
    // Stops if it is the last coords.
    if (pos == 9 * 9)
        return 1;

    // Gets row and col from the current position.
    size_t row = pos / 9;
    size_t col = pos % 9;

    // Got to the next cell if there is already a number in the cell (row, col).
    if (grid[row][col] != '.')
        return solve(grid, pos + 1);
    
    // Tests numbers between 1 and 9 for the cell (row, col).
    for (char possible_nb = '1'; possible_nb < '9' + 1; possible_nb++)
    {
        if (number_is_valid(possible_nb, grid, row, col))
        {
            // Sets the cell (row, col) with the possible number.
            grid[row][col] = possible_nb;
            
            // Checks if the next cell won't have any problem in the grid.
            // Otherwise, continues by setting current cell with the next number.
            if (solve(grid, pos + 1))
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

// Main function.
int main(int argc, char *argv[])
{
    // Exit if there is not only one parameter.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: filepath");
    
    // Creates a new empty grid.
    char grid[9][9] = { 0 };

    // Get the grid from file in one array.
    get_grid_from_file(argv[1], grid);

    // Solves sudoku by checking if the original grid is not valid.
    if (!solve(grid, 0))
        errx(EXIT_FAILURE, "File found, but grid not valid.");
   
    // Saves the result in a new file.
    write_grid_in_file(grid, argv[1]);

    // Exits program with success.
    return EXIT_SUCCESS;
}
