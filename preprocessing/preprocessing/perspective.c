#include "pretreatment.h"
/*
int gaussian_elimination(double *A, double *b, double *x, int n)
{
    for (int i = 0; i < n; ++i) {
        int max_row = i;
        for (int k = i + 1; k < n; ++k) {
            if (fabs(A[k * n + i]) > fabs(A[max_row * n + i])) {
                max_row = k;
            }
        }

        if (fabs(A[max_row * n + i]) < 1e-8) {
            return 1;
        }

        if (i != max_row) {
            for (int j = 0; j < n; ++j) {
                double tmp = A[i * n + j];
                A[i * n + j] = A[max_row * n + j];
                A[max_row * n + j] = tmp;
            }
            double tmp = b[i];
            b[i] = b[max_row];
            b[max_row] = tmp;
        }

        for (int k = i + 1; k < n; ++k) {
            double factor = A[k * n + i] / A[i * n + i];
            for (int j = i; j < n; ++j) {
                A[k * n + j] -= factor * A[i * n + j];
            }
            b[k] -= factor * b[i];
        }
    }

    for (int i = n - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i * n + j] * x[j];
        }
        x[i] /= A[i * n + i];
    }

    return 0;
}

SDL_Surface* perspective_correction(SDL_Surface *input, double corners[4][2]) {
    if (input == NULL) {
        printf("Invalid surface.\n");
        return NULL;
    }

    // Compute the maximum width and height.
    double width_top = sqrt(pow(corners[1][0] - corners[0][0], 2) + pow(corners[1][1] - corners[0][1], 2));
    double width_bottom = sqrt(pow(corners[2][0] - corners[3][0], 2) + pow(corners[2][1] - corners[3][1], 2));
    double height_left = sqrt(pow(corners[3][0] - corners[0][0], 2) + pow(corners[3][1] - corners[0][1], 2));
    double height_right = sqrt(pow(corners[2][0] - corners[1][0], 2) + pow(corners[2][1] - corners[1][1], 2));

    double max_width = (width_top > width_bottom) ? width_top : width_bottom;
    double max_height = (height_left > height_right) ? height_left : height_right;

    // Create the output surface.
    SDL_Surface *output = SDL_CreateRGBSurface(0, (int)max_width, (int)max_height, input->format->BitsPerPixel, input->format->Rmask, input->format->Gmask, input->format->Bmask, input->format->Amask);
    if (output == NULL) {
        printf("Failed to create output surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Compute the homography matrix using Gaussian elimination.
    double A[64] = {
        corners[0][0], corners[0][1], 1, 0, 0, 0, -corners[0][0] * max_width, -corners[0][1] * max_width,
        0, 0, 0, corners[0][0], corners[0][1], 1, -corners[0][0] * 0, -corners[0][1] * 0,
        corners[1][0], corners[1][1], 1, 0, 0, 0, -corners[1][0] * max_width, -corners[1][1] * max_width,
        0, 0, 0, corners[1][0], corners[1][1], 1, -corners[1][0] * max_height, -corners[1][1] * max_height,
        corners[2][0], corners[2][1], 1, 0, 0, 0, -corners[2][0] * 0, -corners[2][1] * 0,
        0, 0, 0, corners[2][0], corners[2][1], 1, -corners[2][0] * max_height, -corners[2][1] * max_height,
        corners[3][0], corners[3][1], 1, 0, 0, 0, -corners[3][0] * 0, -corners[3][1] * 0,
        0, 0, 0, corners[3][0], corners[3][1], 1, -corners[3][0] * 0, -corners[3][1] * 0
    };

    double b[8] = { max_width, 0, max_width, max_height, 0, max_height, 0, 0 };
    double x[8];

    if (gaussian_elimination(A, b, x, 8)) {
        printf("Failed to compute the homography matrix.\n");
        SDL_FreeSurface(output);
        return NULL;
    }

    double H[9] = { x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], 1 };

    // Apply the perspective transform.
    SDL_LockSurface(input);
    SDL_LockSurface(output);

    int output_width = output->w;
    int output_height = output->h;
    Uint32 *input_pixels = (Uint32 *)input->pixels;
    Uint32 *output_pixels = (Uint32 *)output->pixels;

    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
                        double denom = H[6] * x + H[7] * y + H[8];
            double u = (H[0] * x + H[1] * y + H[2]) / denom;
            double v = (H[3] * x + H[4] * y + H[5]) / denom;

            int u_floor = (int)floor(u);
            int v_floor = (int)floor(v);

            if (u_floor >= 0 && u_floor < input->w && v_floor >= 0 && v_floor < input->h) {
                int u_ceil = u_floor + 1;
                int v_ceil = v_floor + 1;

                if (u_ceil >= input->w) u_ceil = input->w - 1;
                if (v_ceil >= input->h) v_ceil = input->h - 1;

                double a = u - u_floor;
                double b = v - v_floor;

                Uint32 color00 = input_pixels[v_floor * input->w + u_floor];
                Uint32 color10 = input_pixels[v_floor * input->w + u_ceil];
                Uint32 color01 = input_pixels[v_ceil * input->w + u_floor];
                Uint32 color11 = input_pixels[v_ceil * input->w + u_ceil];

                Uint8 r00, g00, b00, a00;
                Uint8 r10, g10, b10, a10;
                Uint8 r01, g01, b01, a01;
                Uint8 r11, g11, b11, a11;

                SDL_GetRGBA(color00, input->format, &r00, &g00, &b00, &a00);
                SDL_GetRGBA(color10, input->format, &r10, &g10, &b10, &a10);
                SDL_GetRGBA(color01, input->format, &r01, &g01, &b01, &a01);
                SDL_GetRGBA(color11, input->format, &r11, &g11, &b11, &a11);

                Uint8 red = (1 - a) * (1 - b) * r00 + a * (1 - b) * r10 + (1 - a) * b * r01 + a * b * r11;
                Uint8 green = (1 - a) * (1 - b) * g00 + a * (1 - b) * g10 + (1 - a) * b * g01 + a * b * g11;
                Uint8 blue = (1 - a) * (1 - b) * b00 + a * (1 - b) * b10 + (1 - a) * b * b01 + a * b * b11;
                Uint8 alpha = (1 - a) * (1 - b) * a00 + a * (1 - b) * a10 + (1 - a) * b * a01 + a * b * a11;

                output_pixels[y * output_width + x] = SDL_MapRGBA(output->format, red, green, blue, alpha);
            }
        }
    }

    SDL_UnlockSurface(input);
    SDL_UnlockSurface(output);

    return output;
}


SDL_Surface* perspective_correction(SDL_Surface *input, double corners[4][2]) {
    if (input == NULL) {
        printf("Invalid surface.\n");
        return NULL;
    }

    // Compute the maximum width and height.
    double width_top = sqrt(pow(corners[1][0] - corners[0][0], 2) + pow(corners[1][1] - corners[0][1], 2));
    double width_bottom = sqrt(pow(corners[2][0] - corners[3][0], 2) + pow(corners[2][1] - corners[3][1], 2));
    double height_left = sqrt(pow(corners[3][0] - corners[0][0], 2) + pow(corners[3][1] - corners[0][1], 2));
    double height_right = sqrt(pow(corners[2][0] - corners[1][0], 2) + pow(corners[2][1] - corners[1][1], 2));

    double max_width = (width_top > width_bottom) ? width_top : width_bottom;
    double max_height = (height_left > height_right) ? height_left : height_right;

    // Create the output surface.
    SDL_Surface *output = SDL_CreateRGBSurface(0, (int)max_width, (int)max_height, input->format->BitsPerPixel, input->format->Rmask, input->format->Gmask, input->format->Bmask, input->format->Amask);
    if (output == NULL) {
        printf("Failed to create output surface: %s\n", SDL_GetError());
        return NULL;
    }

   // Compute the homography matrix using Gaussian elimination.
    double A[64] = {
        corners[0][0], corners[0][1], 1, 0, 0, 0, -corners[0][0] * 0, -corners[0][1] * 0,
        0, 0, 0, corners[0][0], corners[0][1], 1, -corners[0][0] * 0, -corners[0][1] * 0,
        corners[1][0], corners[1][1], 1, 0, 0, 0, -corners[1][0] * max_width, -corners[1][1] * max_width,
        0, 0, 0, corners[1][0], corners[1][1], 1, -corners[1][0] * 0, -corners[1][1] * 0,
        corners[2][0], corners[2][1], 1, 0, 0, 0, -corners[2][0] * max_width, -corners[2][1] * max_width,
        0, 0, 0, corners[2][0], corners[2][1], 1, -corners[2][0] * max_height, -corners[2][1] * max_height,
        corners[3][0], corners[3][1], 1, 0, 0, 0, -corners[3][0] * 0, -corners[3][1] * 0,
        0, 0, 0, corners[3][0], corners[3][1], 1, -corners[3][0] * max_height, -corners[3][1] * max_height
    };

    double b[8] = { 0, 0, max_width, 0, max_width, max_height, 0, max_height };
    double x[8];

    if (gaussian_elimination(A, b, x, 8)) {
        printf("Failed to compute the homography matrix.\n");
        SDL_FreeSurface(output);
        return NULL;
    }

    double H[9] = { x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], 1 };

    // Apply the perspective transform.
    SDL_LockSurface(input);
    SDL_LockSurface(output);

    int output_width = output->w;
    int output_height = output->h;
    Uint32 *input_pixels = (Uint32 *)input->pixels;
    Uint32 *output_pixels = (Uint32 *)output->pixels;

    for (int y = 0; y < output_height; y++) {
        for (int x = 0; x < output_width; x++) {
            double denom = H[6] * x + H[7] * y + H[8];
            double u = (H[0] * x + H[1] * y + H[2]) / denom;
            double v = (H[3] * x + H[4] * y + H[5]) / denom;

            int u_floor = (int)floor(u);
            int v_floor = (int)floor(v);

                        if (u_floor >= 0 && u_floor < input->w && v_floor >= 0 && v_floor < input->h) {
                int u_ceil = u_floor + 1;
                int v_ceil = v_floor + 1;

                if (u_ceil >= input->w) u_ceil = input->w - 1;
                if (v_ceil >= input->h) v_ceil = input->h - 1;

                double a = u - u_floor;
                double b = v - v_floor;

                Uint32 color00 = input_pixels[v_floor * input->w + u_floor];
                Uint32 color10 = input_pixels[v_floor * input->w + u_ceil];
                Uint32 color01 = input_pixels[v_ceil * input->w + u_floor];
                Uint32 color11 = input_pixels[v_ceil * input->w + u_ceil];

                Uint8 r00, g00, b00, a00;
                Uint8 r10, g10, b10, a10;
                Uint8 r01, g01, b01, a01;
                Uint8 r11, g11, b11, a11;

                SDL_GetRGBA(color00, input->format, &r00, &g00, &b00, &a00);
                SDL_GetRGBA(color10, input->format, &r10, &g10, &b10, &a10);
                SDL_GetRGBA(color01, input->format, &r01, &g01, &b01, &a01);
                SDL_GetRGBA(color11, input->format, &r11, &g11, &b11, &a11);

                Uint8 red = (1 - a) * (1 - b) * r00 + a * (1 - b) * r10 + (1 - a) * b * r01 + a * b * r11;
                Uint8 green = (1 - a) * (1 - b) * g00 + a * (1 - b) * g10 + (1 - a) * b * g01 + a * b * g11;
                Uint8 blue = (1 - a) * (1 - b) * b00 + a * (1 - b) * b10 + (1 - a) * b * b01 + a * b * b11;
                Uint8 alpha = (1 - a) * (1 - b) * a00 + a * (1 - b) * a10 + (1 - a) * b * a01 + a * b * a11;

                output_pixels[y * output_width + x] = SDL_MapRGBA(output->format, red, green, blue, alpha);
            }
        }
    }

    SDL_UnlockSurface(input);
    SDL_UnlockSurface(output);

    return output;
}
*/
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
            coutMatrix[row][col] =
                cofactorMatrix[col][row] / determinte; // adjoint method
        }
    }
    return;
}

void Cofactor(double cinMatrix[9][9], double sizeMatrix, double determinte,
              double coutMatrix[9][9], double transposeMatrix[9][9])
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
    Transpose(cofactorMatrix, sizeMatrix, determinte, coutMatrix,
              transposeMatrix); // function
    return;
}

void Inverse(double cinMatrix[9][9], int sizeMatrix, double determinte,
             double coutMatrix[9][9], double transposeMatrix[9][9])
{
    if (determinte == 0)
    {
        printf("\n Inverse of entered matrix is not possible \n");
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

void perspectiveMatrix(int src[4][2], double dst[4][2], double **transformation_matrix, double **transformation_matrix_inv)
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

SDL_Surface* perspective_transform(SDL_Surface* surface, int corners[4][2])
{
    int top = sqrt(pow(corners[1][0] - corners[0][0], 2) + pow(corners[1][1] - corners[0][1], 2));
    int bottom = sqrt(pow(corners[2][0] - corners[3][0], 2) + pow(corners[2][1] - corners[3][1], 2));
    int left = sqrt(pow(corners[3][0] - corners[0][0], 2) + pow(corners[3][1] - corners[0][1], 2));
    int right = sqrt(pow(corners[2][0] - corners[1][0], 2) + pow(corners[2][1] - corners[1][1], 2));

    double max_edge_length = fmax(fmax(top, right), fmax(bottom, left));

    double dst[4][2] = { { 0, 0 },
                         { max_edge_length, 0 },
                         { max_edge_length, max_edge_length },
                         { 0, max_edge_length } };

    double **transformationMat = allocMat(3);

    double **transformationMatInv = allocMat(3);

    perspectiveMatrix(corners, dst, transformationMat, transformationMatInv);

    //Image corrected_image = newImage(NULL, 0, max_edge_length, max_edge_length);
    SDL_Surface* corrected_image = SDL_CreateRGBSurface(0, max_edge_length, max_edge_length, 32, 0, 0, 0, 0);

    // Ensure both surfaces are locked so that we can safely access raw pixel data
    if (SDL_MUSTLOCK(corrected_image)) SDL_LockSurface(corrected_image);
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
    
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
		// Calculate correct pixel index
		Uint32 src_index = y * surface->pitch/4 + x;
		Uint32 dst_index = j * corrected_image->pitch/4 + i;
		
		// Copy pixel data
		dst_pixels[dst_index] = src_pixels[src_index];
	      }
	    else
	      {
		// If out of bounds, set pixel to black
		Uint32 dst_index = j * corrected_image->pitch/4 + i;
		dst_pixels[dst_index] = SDL_MapRGBA(corrected_image->format, 0, 0, 0, 255);
	      }
	  }
      }
    
    // Unlock surfaces
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    if (SDL_MUSTLOCK(corrected_image)) SDL_UnlockSurface(corrected_image);
    
    freeMat(transformationMat, 3);
    freeMat(transformationMatInv, 3);

    return corrected_image;
}
