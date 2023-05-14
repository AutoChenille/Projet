#include "pretreatment.h"

void minor(double matrix_minor[9][9], int column, int size, double new_matrix_minor[9][9])
{
    int col = 0, row = 0;

    for (int i = 1; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (j == column)
                continue;
            new_matrix_minor[row][col] = matrix_minor[i][j];
            col++;
            
            if (col == (size - 1))
            {
                row++;
                col = 0;
            }
        }
    }
}

double determining(double matrix_minor[9][9], int size)
{
    int col;
    double sum = 0, new_matrix_minor[9][9];

    if (size == 1)
        return matrix_minor[0][0];
    else if (size == 2)
        return (matrix_minor[0][0] * matrix_minor[1][1] - matrix_minor[0][1] * matrix_minor[1][0]);
    else
    {
        for (col = 0; col < size; col++)
        {
            minor(matrix_minor, col, size, new_matrix_minor);
            sum += (double)(matrix_minor[0][col] * pow(-1, col) * determining(new_matrix_minor, (size - 1)));
        }
    }

    return sum;
}

void transpose_matrix(double cofactorMatrix[9][9], double size, double delta, double coutMatrix[9][9], double transposeMatrix[9][9])
{
    int row, col;
    for (row = 0; row < size; row++)
    {
        for (col = 0; col < size; col++)
        {
            transposeMatrix[row][col] = cofactorMatrix[col][row];
            coutMatrix[row][col] = cofactorMatrix[col][row] / delta;
        }
    }
}

void cofactor(double cinMatrix[9][9], double size, double determinte, double c_matrix[9][9], double t_matrix[9][9])
{
    double minor_matrix[9][9], cofactor_matrix[9][9];
    int col3, row3, row2, col2, row, col;

    for (row3 = 0; row3 < size; row3++)
    {
        for (col3 = 0; col3 < size; col3++)
        {
            row2 = 0, col2 = 0;
            for (row = 0; row < size; row++)
            {
                for (col = 0; col < size; col++)
                {
                    if (row != row3 && col != col3)
                    {
                        minor_matrix[row2][col2] = cinMatrix[row][col];
                        
                        if (col2 < (size - 2))
                        {
                            col2++;
                        }
                        else
                        {
                            col2 = 0;
                            row2++;
                        }
                    }
                }
            }
            cofactor_matrix[row3][col3] = pow(-1, (row3 + col3)) * determining(minor_matrix, (size - 1));
        }
    }
    transpose_matrix(cofactor_matrix, size, determinte, c_matrix, t_matrix);
}

void inverse(double matrix[9][9], int size, double delta, double c_matrix[9][9], double t_matrix[9][9])
{
    if (delta == 0)
        errx(1, "No inverse for the given Matrix\n");
    else if (size == 1)
        c_matrix[0][0] = 1;
    else
        cofactor(matrix, size, delta, c_matrix, t_matrix);
}

void inverse_matrix(double matrix[9][9], double c_matrix[9][9], int size)
{
    double delta, t_matrix[9][9];

    delta = (double)determining(matrix, size);
    
    inverse(matrix, size, delta, c_matrix, t_matrix);
}

double** malloc_matrix(int size)
{
    double **matrix = calloc(size, sizeof(double *));
    
    if (matrix == NULL)
    {
        errx(1, "malloc_matrix\n");
    }
    
    for (int i = 0; i < size; i++)
    {
        matrix[i] = calloc(size, sizeof(double));
        
        if (matrix[i] == NULL)
	    {
            errx(1, "malloc_matrix\n");
        }
    }
    
    return matrix;
}

void multiply_matrix_normal(double matrix[9][9], double column[9], double res[9], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            res[i] += matrix[i][j] * column[j];
        }
    }
}

void multiply_matrix_inverse(double **matrix, double *column, double *res, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            res[i] += matrix[i][j] * column[j];
}

void free_matrix(double** matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
}

void inverse_matrix_3(double **M, double **matrix_inverse)
{
    double MM = M[0][0] * M[1][1] * M[2][2] + M[0][1] * M[1][2] * M[2][0] + M[0][2] * M[2][1] * M[1][0] - M[0][2] * M[1][1] * M[2][0]
      - M[0][1] * M[1][0] * M[2][2] - M[0][0] * M[2][1] * M[1][2];

    double AM[3][3] = {{M[1][1] * M[2][2] - M[1][2] * M[2][1], M[0][2] * M[2][1] - M[0][1] * M[2][2], M[0][1] * M[1][2] - M[0][2] * M[1][1]},
		       {M[1][2] * M[2][0] - M[1][0] * M[2][2], M[0][0] * M[2][2] - M[0][2] * M[2][0], M[0][2] * M[1][0] - M[0][0] * M[1][2]},
		       {M[1][0] * M[2][1] - M[1][1] * M[2][0], M[0][1] * M[2][0] - M[0][0] * M[2][1], M[0][0] * M[1][1] - M[0][1] * M[1][0]}};

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            matrix_inverse[i][j] = AM[i][j] / MM;
}

// Compute the cross product
void cross_product(double matrix1[], double matrix2[], double cross_product[])
{
    cross_product[0] = matrix1[1] * matrix2[2] - matrix1[2] * matrix2[1];
    cross_product[1] = matrix1[2] * matrix2[0] - matrix1[0] * matrix2[2];
    cross_product[2] = matrix1[0] * matrix2[1] - matrix1[1] * matrix2[0];
}

void get_perspective_matrix(double src[4][2], double dst[4][2], double **transformation_matrix, double **transformation_matrix_inverse)
{
    // Create perspective matrix
    double perspective_matrix[][9] = {{-src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0], src[0][1] * dst[0][0], dst[0][0]},
		                              {0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1], src[0][1] * dst[0][1], dst[0][1]},
		                              {-src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0], src[1][1] * dst[1][0], dst[1][0]},
		                              {0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1], src[1][1] * dst[1][1], dst[1][1]},
		                              {-src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0], src[2][1] * dst[2][0], dst[2][0]},
		                              {0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1],  src[2][1] * dst[2][1], dst[2][1]},
		                              {-src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0], src[3][1] * dst[3][0], dst[3][0]},
		                              {0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1],  src[3][1] * dst[3][1], dst[3][1]},
		                              {0, 0, 0, 0, 0, 0, 0, 0, 1}};

    // Create the inverse of the perspective matrix
    double perspective_matrix_inverse[9][9] = {0};

    double R[9] = {0, 0, 0, 0, 0, 0, 0, 0, 1};
    double* homography = calloc(9, sizeof(double));

    // Compute the inverse of matrix P
    inverse_matrix(perspective_matrix, perspective_matrix_inverse, 9);

    // Compute homography = perspective_matrix_inverse * R
    multiply_matrix_normal(perspective_matrix_inverse, R, homography, 9);

    // Convert homography to a matrix with 3 rows and 3 columns
    int k = 0;
    size_t rows = 3, cols = 3;

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            transformation_matrix[i][j] = homography[k];
            k++;
        }
    }

    inverse_matrix_3(transformation_matrix, transformation_matrix_inverse);
    
    free(homography);
}

SDL_Surface* perspective_transform(SDL_Surface* surface, double corners[4][2])
{
    // Calculate the length of each sudoku's side
    int top = sqrt(pow(corners[1][0] - corners[0][0], 2) + pow(corners[1][1] - corners[0][1], 2));
    int right = sqrt(pow(corners[2][0] - corners[1][0], 2) + pow(corners[2][1] - corners[1][1], 2));
    int bottom = sqrt(pow(corners[2][0] - corners[3][0], 2) + pow(corners[2][1] - corners[3][1], 2));
    int left = sqrt(pow(corners[3][0] - corners[0][0], 2) + pow(corners[3][1] - corners[0][1], 2));

    // Get the length of the largest side
    double length = fmax(fmax(top, right), fmax(bottom, left));

    // Calculate the new coordinates of the 4 corners
    double dst[4][2] = {{0, 0},	{length, 0}, {length, length}, {0, length}};

    // Malloc the matrix and matrix inverse
    double **trans_matrix = malloc_matrix(3);
    double **trans_matrix_inverse = malloc_matrix(3);

    // Get the perspective matrix
    get_perspective_matrix(corners, dst, trans_matrix, trans_matrix_inverse);

    // Create the result surface with the correct perspective
    SDL_Surface* perspective_surface = SDL_CreateRGBSurfaceWithFormat(0, length, length, 32, SDL_PIXELFORMAT_RGBA32);

    // Ensure the surface format is correct
    perspective_surface = SDL_ConvertSurfaceFormat(perspective_surface, SDL_PIXELFORMAT_RGB888, 0);
    
    // Ensure both surfaces are locked so that we can safely access raw pixel data
    SDL_LockSurface(surface);
    SDL_LockSurface(perspective_surface);
 
    // Cast pixel data to the correct type
    Uint32* src_pixels = (Uint32*) surface->pixels;
    Uint32* dst_pixels = (Uint32*) perspective_surface->pixels;

    // Main algorithm
    for (size_t i = 0; i < length; i++)
    {
	    for (size_t j = 0; j < length; j++)
	    {	    
	        double old_coords[3] = {(double)i, (double)j, 1};
	        double new_coords[3] = {0, 0, 0};
	    
	        multiply_matrix_inverse(trans_matrix_inverse, old_coords, new_coords, 3);
	    
	        int x = (int)(new_coords[0] / new_coords[2]);
	        int y = (int)(new_coords[1] / new_coords[2]);
	    
	        if (x >= 0 && y >= 0 && x < surface->w && y < surface->h)
	        {
		        // Calculate correct pixel index
		        Uint32 src_index = y * surface->pitch/4 + x;
		        Uint32 dst_index = j * perspective_surface->pitch/4 + i;
		
		        // Copy pixel data
		        dst_pixels[dst_index] = src_pixels[src_index];
	        }
	        else
	        {
	            // If out of bounds, set pixel to black (default)
		        Uint32 dst_index = j * perspective_surface->pitch/4 + i;

		        dst_pixels[dst_index] = SDL_MapRGBA(perspective_surface->format, 0, 0, 0, 255);
	        }
        }
    }

    // Unlock surfaces
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(perspective_surface);

    // Free matrices
    free_matrix(trans_matrix, 3);
    free_matrix(trans_matrix_inverse, 3);

    // Return the corrected perspective surface
    return perspective_surface;
}
