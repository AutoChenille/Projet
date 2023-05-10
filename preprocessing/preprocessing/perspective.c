#include "pretreatment.h"

void Minor(double minorMatrix[9][9], int colMatrix, int sizeMatrix,
           double newMinorMatrix[9][9])
{
    int col, row, row2 = 0, col2 = 0;
    for (row = 1; row < sizeMatrix; row++)
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            if (col == colMatrix)
            {
                continue;
            }
            newMinorMatrix[row2][col2] = minorMatrix[row][col];
            col2++;
            if (col2 == (sizeMatrix - 1))
            {
                row2++;
                col2 = 0;
            }
        }
    }
    return;
}

double Determinte(double minorMatrix[9][9], int sizeMatrix)
{
    int col;
    double sum = 0, newMinorMatrix[9][9];
    if (sizeMatrix == 1)
    {
        return minorMatrix[0][0];
    }
    else if (sizeMatrix == 2)
    {
        return (minorMatrix[0][0] * minorMatrix[1][1]
                - minorMatrix[0][1] * minorMatrix[1][0]);
    }
    else
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            Minor(minorMatrix, col, sizeMatrix, newMinorMatrix); // function
            sum += (double)(minorMatrix[0][col] * pow(-1, col)
                            * Determinte(newMinorMatrix,
                                         (sizeMatrix - 1))); // function
        }
    }
    return sum;
}

void Transpose(double cofactorMatrix[9][9], double sizeMatrix,
               double determinte, double coutMatrix[9][9],
               double transposeMatrix[9][9])
{
    int row, col;
    for (row = 0; row < sizeMatrix; row++)
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            transposeMatrix[row][col] = cofactorMatrix[col][row];
            coutMatrix[row][col] = cofactorMatrix[col][row] / determinte; // adjoint method
        }
    }
    return;
}

void Cofactor(double cinMatrix[9][9], double sizeMatrix, double determinte, double coutMatrix[9][9], double transposeMatrix[9][9])
{
    double minorMatrix[9][9], cofactorMatrix[9][9];
    int col3, row3, row2, col2, row, col;
    for (row3 = 0; row3 < sizeMatrix; row3++)
    {
        for (col3 = 0; col3 < sizeMatrix; col3++)
        {
            row2 = 0;
            col2 = 0;
            for (row = 0; row < sizeMatrix; row++)
            {
                for (col = 0; col < sizeMatrix; col++)
                {
                    if (row != row3 && col != col3)
                    {
                        minorMatrix[row2][col2] = cinMatrix[row][col];
                        if (col2 < (sizeMatrix - 2))
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
            cofactorMatrix[row3][col3] = pow(-1, (row3 + col3))
                * Determinte(minorMatrix, (sizeMatrix - 1));
        }
    }
    Transpose(cofactorMatrix, sizeMatrix, determinte, coutMatrix, transposeMatrix);
    return;
}

void Inverse(double cinMatrix[9][9], int sizeMatrix, double determinte, double coutMatrix[9][9], double transposeMatrix[9][9])
{
    if (determinte == 0)
    {
        printf("Inverse of entered matrix is not possible\n");
    }
    else if (sizeMatrix == 1)
    {
        coutMatrix[0][0] = 1;
    }
    else
    {
        Cofactor(cinMatrix, sizeMatrix, determinte, coutMatrix,
                 transposeMatrix); // function
    }
    return;
}

void inverseMat(double cinMatrix[9][9], double coutMatrix[9][9], int sizeMatrix)
{
    double determinte, transposeMatrix[9][9];

    determinte = (double)Determinte(cinMatrix, sizeMatrix);

    Inverse(cinMatrix, sizeMatrix, determinte, coutMatrix, transposeMatrix);
}

double** allocMat(int size)
{
    double **mat = calloc(size, sizeof(double *));
    if (mat == NULL)
    {
        printf("Error allocating memory for matrix\n");
        exit(1);
    }
    for (int i = 0; i < size; i++)
    {
        mat[i] = calloc(size, sizeof(double));
        if (mat[i] == NULL)
        {
            printf("Error allocating memory for matrix\n");
            exit(1);
        }
    }
    return mat;
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
}

void multiplyMatBis(double **M, double *v, double *v_out, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            v_out[i] += M[i][j] * v[j];
        }
    }
}

void freeMat(double **mat, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

void inverse3x3Mat(double **M, double **M_inv)
{
    double MM = M[0][0] * M[1][1] * M[2][2] + M[0][1] * M[1][2] * M[2][0]
        + M[0][2] * M[2][1] * M[1][0] - M[0][2] * M[1][1] * M[2][0]
        - M[0][1] * M[1][0] * M[2][2] - M[0][0] * M[2][1] * M[1][2];

    double AM[3][3] = { { M[1][1] * M[2][2] - M[1][2] * M[2][1],
                          M[0][2] * M[2][1] - M[0][1] * M[2][2],
                          M[0][1] * M[1][2] - M[0][2] * M[1][1] },
                        { M[1][2] * M[2][0] - M[1][0] * M[2][2],
                          M[0][0] * M[2][2] - M[0][2] * M[2][0],
                          M[0][2] * M[1][0] - M[0][0] * M[1][2] },
                        { M[1][0] * M[2][1] - M[1][1] * M[2][0],
                          M[0][1] * M[2][0] - M[0][0] * M[2][1],
                          M[0][0] * M[1][1] - M[0][1] * M[1][0] } };

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

void perspectiveMatrix(double src[4][2], double dst[4][2], double **transformation_matrix, double **transformation_matrix_inv)
{
    double P[][9] = {
        { -src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0],
          src[0][1] * dst[0][0], dst[0][0] },
        { 0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1],
          src[0][1] * dst[0][1], dst[0][1] },
        { -src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0],
          src[1][1] * dst[1][0], dst[1][0] },
        { 0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1],
          src[1][1] * dst[1][1], dst[1][1] },
        { -src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0],
          src[2][1] * dst[2][0], dst[2][0] },
        { 0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1],
          src[2][1] * dst[2][1], dst[2][1] },
        { -src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0],
          src[3][1] * dst[3][0], dst[3][0] },
        { 0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1],
          src[3][1] * dst[3][1], dst[3][1] },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1 }
    };

    double R[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    double P_inv[9][9] = { 0 };

    // Invert matrix P
    inverseMat(P, P_inv, 9);

    // Compute H = P_inv * R
    double *H = calloc(9, sizeof(double));
    multiplyMatStat(P_inv, R, H, 9);

    // Convert H to 3x3 matrix
    int v = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++, v++)
        {
            transformation_matrix[i][j] = H[v];
        }
    }

    inverse3x3Mat(transformation_matrix, transformation_matrix_inv);

    free(H);
}

SDL_Surface* perspective_transform(SDL_Surface* surface, double corners[4][2])
{
    int top = sqrt(pow(corners[1][0] - corners[0][0], 2) + pow(corners[1][1] - corners[0][1], 2));
    int right = sqrt(pow(corners[2][0] - corners[1][0], 2) + pow(corners[2][1] - corners[1][1], 2));
    int bottom = sqrt(pow(corners[2][0] - corners[3][0], 2) + pow(corners[2][1] - corners[3][1], 2));
    int left = sqrt(pow(corners[3][0] - corners[0][0], 2) + pow(corners[3][1] - corners[0][1], 2));
    
    printf("top = %i\n", top);
    printf("right = %i\n", right);
    printf("bottom = %i\n", bottom);
    printf("left = %i\n", left);
    

    double max_edge_length = fmax(fmax(top, right), fmax(bottom, left));
    printf("max_edge_length = %f\n", max_edge_length);

    double dst[4][2] = { { 0, 0 },
                         { max_edge_length, 0 },
                         { max_edge_length, max_edge_length },
                         { 0, max_edge_length } };

    double **transformationMat = allocMat(3);

    double **transformationMatInv = allocMat(3);

    perspectiveMatrix(corners, dst, transformationMat, transformationMatInv);

    //Image corrected_image = newImage(NULL, 0, max_edge_length, max_edge_length);
    
    SDL_Surface* corrected_image = SDL_CreateRGBSurfaceWithFormat(0, max_edge_length, max_edge_length, 32, SDL_PIXELFORMAT_RGBA32);
    printf("corrected_image->w = %i\n", corrected_image->w);
    printf("corrected_image->h = %i\n", corrected_image->h);

    SDL_PixelFormat* desired_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    surface = SDL_ConvertSurface(surface, desired_format, 0);
    
    // Ensure both surfaces are locked so that we can safely access raw pixel data
    SDL_LockSurface(surface);
    SDL_LockSurface(corrected_image);
 
    // Cast pixel data to the correct type
    Uint32* src_pixels = (Uint32*) surface->pixels;
    Uint32* dst_pixels = (Uint32*) corrected_image->pixels;
    
    for (unsigned int i = 0; i < max_edge_length; i++)
      {
	for (unsigned int j = 0; j < max_edge_length; j++)
	  {
	    double ut = i;
	    double vt = j;
	    double wt = 1;
	    
	    double old_coordinates[3] = { ut, vt, wt };
	    double new_coordinates[3] = { 0, 0, 0 };
	    
	    multiplyMatBis(transformationMatInv, old_coordinates, new_coordinates, 3);
	    
	    int x = (int)(new_coordinates[0] / new_coordinates[2]);
	    int y = (int)(new_coordinates[1] / new_coordinates[2]);
	    
	    if (x >= 0 && y >= 0 && x < surface->w && y < surface->h)
	      {
		printf("IF statement\n");
		// Calculate correct pixel index
		Uint32 src_index = y * surface->pitch/4 + x;
		Uint32 dst_index = j * corrected_image->pitch/4 + i;
		
		// Copy pixel data
		dst_pixels[dst_index] = src_pixels[src_index];
	      }
	    else
	      {
		printf("ELSE statement\n");
		// If out of bounds, set pixel to black
		Uint32 dst_index = j * corrected_image->pitch/4 + i;
		dst_pixels[dst_index] = SDL_MapRGBA(corrected_image->format, 0, 0, 0, 255);
	      }
	  }
      }
    printf("Fini!\n");
    // Unlock surfaces
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    if (SDL_MUSTLOCK(corrected_image)) SDL_UnlockSurface(corrected_image);
    
    freeMat(transformationMat, 3);
    freeMat(transformationMatInv, 3);

    return corrected_image;
}
