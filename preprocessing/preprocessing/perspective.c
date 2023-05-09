#include "pretreatment.h"


int gaussian_elimination(double *A, double *b, double *x, int n) {
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
/*
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
*/

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

