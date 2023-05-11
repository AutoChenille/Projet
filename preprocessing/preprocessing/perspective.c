#include "pretreatment.h"

void Minor(double minorMatrix[9][9], int colMatrix, int size, double newMinorMatrix[9][9])
{
    int col = 0;
    int row = 0;

    for (int i = 1; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (j == colMatrix)
                continue;
            newMinorMatrix[row][col] = minorMatrix[i][j];
            col++;
            if (col == (size - 1))
            {
                row++;
                col = 0;
            }
        }
    }

    return;
}

double determining(double minorMatrix[9][9], int size)
{
    int col;
    double sum = 0, newMinorMatrix[9][9];

    if (size == 1)
        return minorMatrix[0][0];
    else if (size == 2)
        return (minorMatrix[0][0] * minorMatrix[1][1] - minorMatrix[0][1] * minorMatrix[1][0]);
    else
    {
        for (col = 0; col < size; col++)
        {
            Minor(minorMatrix, col, size, newMinorMatrix);
            sum += (double)(minorMatrix[0][col] * pow(-1, col) * determining(newMinorMatrix, (size - 1)));
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
    return;
}

void Cofactor(double cinMatrix[9][9], double size, double determinte, double coutMatrix[9][9], double transposeMatrix[9][9])
{
    double minorMatrix[9][9], cofactorMatrix[9][9];
    int col3, row3, row2, col2, row, col;

    for (row3 = 0; row3 < size; row3++)
    {
        for (col3 = 0; col3 < size; col3++)
        {
            row2 = 0;
            col2 = 0;
            for (row = 0; row < size; row++)
            {
                for (col = 0; col < size; col++)
                {
                    if (row != row3 && col != col3)
                    {
                        minorMatrix[row2][col2] = cinMatrix[row][col];
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
            cofactorMatrix[row3][col3] = pow(-1, (row3 + col3)) * determining(minorMatrix, (size - 1));
        }
    }
    transpose_matrix(cofactorMatrix, size, determinte, coutMatrix, transposeMatrix);

    return;
}

void inverse(double cinMatrix[9][9], int size, double delta, double coutMatrix[9][9], double transposeMatrix[9][9])
{
    if (delta == 0)
        printf("Inverse of entered matrix is not possible\n");
    else if (size == 1)
        coutMatrix[0][0] = 1;
    else
        Cofactor(cinMatrix, size, delta, coutMatrix, transposeMatrix);

    return;
}

void inverse_matrix(double cinMatrix[9][9], double coutMatrix[9][9], int size)
{
    double delta, transposeMatrix[9][9];

    delta = (double)determining(cinMatrix, size);
    inverse(cinMatrix, size, delta, coutMatrix, transposeMatrix);
}

double** malloc_matrix(int size)
{
    double **matrix = calloc(size, sizeof(double *));
    
    if (matrix == NULL)
    {
        printf("Error malloc matrix at malloc_matrix\n");
        exit(1);
    }
    
    for (int i = 0; i < size; i++)
    {
        matrix[i] = calloc(size, sizeof(double));
        if (matrix[i] == NULL)
	{
            printf("Error malloc matrix at malloc_matrix\n");
            exit(1);
        }
    }
    
    return matrix;
}

void multiplyMatStat(double M[9][9], double v[9], double v_out[9], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            v_out[i] += M[i][j] * v[j];
        }
    }
    return;
}

void multiplyMatBis(double **M, double *v, double *v_out, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            v_out[i] += M[i][j] * v[j];

    return;
}

void free_matrix(double** matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
}

void inverse_matrix_3(double **M, double **M_inv)
{
    double MM = M[0][0] * M[1][1] * M[2][2] + M[0][1] * M[1][2] * M[2][0] + M[0][2] * M[2][1] * M[1][0] - M[0][2] * M[1][1] * M[2][0]
      - M[0][1] * M[1][0] * M[2][2] - M[0][0] * M[2][1] * M[1][2];

    double AM[3][3] = {{M[1][1] * M[2][2] - M[1][2] * M[2][1], M[0][2] * M[2][1] - M[0][1] * M[2][2], M[0][1] * M[1][2] - M[0][2] * M[1][1]},
		       {M[1][2] * M[2][0] - M[1][0] * M[2][2], M[0][0] * M[2][2] - M[0][2] * M[2][0], M[0][2] * M[1][0] - M[0][0] * M[1][2]},
		       {M[1][0] * M[2][1] - M[1][1] * M[2][0], M[0][1] * M[2][0] - M[0][0] * M[2][1], M[0][0] * M[1][1] - M[0][1] * M[1][0]}};

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            M_inv[i][j] = AM[i][j] / MM;
}

void crossProduct(double vect_A[], double vect_B[], double cross_P[])
{
    cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
    cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
}

void get_perspective_matrix(double src[4][2], double dst[4][2], double **transformation_matrix, double **transformation_matrix_inv)
{
    // Create perspective matrix
    double P[][9] = {{-src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0], src[0][1] * dst[0][0], dst[0][0]},
		     {0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1], src[0][1] * dst[0][1], dst[0][1]},
		     {-src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0], src[1][1] * dst[1][0], dst[1][0]},
		     {0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1], src[1][1] * dst[1][1], dst[1][1]},
		     {-src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0], src[2][1] * dst[2][0], dst[2][0]},
		     {0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1],  src[2][1] * dst[2][1], dst[2][1]},
		     {-src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0], src[3][1] * dst[3][0], dst[3][0]},
		     {0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1],  src[3][1] * dst[3][1], dst[3][1]},
		     {0, 0, 0, 0, 0, 0, 0, 0, 1}};

    // Create the inverse of the perspective matrix
    double P_inv[9][9] = {0};

    double R[9] = {0, 0, 0, 0, 0, 0, 0, 0, 1};

    // Compute the inverse of matrix P
    inverse_matrix(P, P_inv, 9);

    double *H = calloc(9, sizeof(double));

    // Compute H = P_inv * R
    multiplyMatStat(P_inv, R, H, 9);

    // Convert H to a matrix with 3 rows and 3 columns
    int k = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++, k++)
            transformation_matrix[i][j] = H[k];

    inverse_matrix_3(transformation_matrix, transformation_matrix_inv);
    free(H);
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
    double **transformationMat = malloc_matrix(3);
    double **transformationMatInv = malloc_matrix(3);

    // Get the perspective matrix
    get_perspective_matrix(corners, dst, transformationMat, transformationMatInv);

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
	    
	    multiplyMatBis(transformationMatInv, old_coords, new_coords, 3);
	    
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
    free_matrix(transformationMat, 3);
    free_matrix(transformationMatInv, 3);

    return perspective_surface;
}
