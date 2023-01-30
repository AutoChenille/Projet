#include "SDL_utilities.h"

// Loads image and gets its surface.
//
// path: path of the image
// return: surface of the image.
SDL_Surface* load_image(const char path[])
{
    // Creates a new surface from the image.
    SDL_Surface* surf = IMG_Load(path);

    // Check if there is an error with the surface.
    if (surf == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Returns the image created with the right format.
    return surf;
}

// Converts an array (especially the accumulator) in surface.
//
// w: width of image.
// h: height of image.
// accumulator: the accumulator made in hough_transform to convert in surface.
// return: the surface created from array.
SDL_Surface* array_to_surface(int w, int h, int accumulator[])
{
    // Creates new surface with right width and height.
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

    // Lock surface surf.
    SDL_LockSurface(surf);

    // Get maximum in accumulator for tree rule.
    int max_acc = max_array(accumulator, w * h);

    // Get array of pixels of type Uint32.
    Uint32* pixels = surf->pixels;

    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            // Gets the value in accumulator.
            int acc_pixel = accumulator[i + w * j];

            // Computes pixel value for the color.
            int pixel_value = 255 * acc_pixel / max_acc;

            // Assigns the new value of pixel.
            Uint8 val = (pixel_value > 255) ? 255 : pixel_value;

            // Attributes pixel value.
            pixels[i + w * j] = SDL_MapRGB(surf->format, val, val, val);
        }
    }

    // Unlock surface surf.
    SDL_UnlockSurface(surf);

    return surf;
}

// Converts a pixel color into an int bit value.
// Original image must be a black and white image.
//
// surf: original surface the color pixel is coming from.
// color: color of one pixel.
// return: 0 for black, 1 for white.
int pixel_to_bit_value(SDL_Surface* surf, Uint32 color)
{
    // Will use only red value.
    // We do not need the other because it is a black and white image.

    Uint8 r, g, b;

    // Saves value of red color in variable r.
    SDL_GetRGB(color, surf->format, &r, &g, &b);

    // Returns if r correspond to black or white (0: black, 1: white).
    // Detects if 'r' is the null character for black or not.
    // Uint8 is like a char.
    return (char) r == '\0' ? 0 : 1;
}

// From a surface, gets a matrix with 0 and 1 (0: black, 1: white)
//
// surf: surface from an image.
// pixel_bit_array: it is the destination (the result array will be stored into).
// return: nothing.
void get_array_of_pixels(SDL_Surface* surf, int pixel_bit_array[])
{
    // Gets the length and the width of the surface.
    int width = surf->w;
    int height = surf->h;

    // Locks surface.
    SDL_LockSurface(surf);

    // Get array of pixels of type Uint32.
    Uint32* pixels = surf->pixels;

    // Stores all the pixels of the surface in one int array.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Gets index in array.
            int index = j + width * i;

            // Gets color of pixel.
            int bit = pixel_to_bit_value(surf, pixels[index]);

            // Assigns bit to array (0 or 1).
            pixel_bit_array[index] = bit;
        }
    }

    // Unlocks surface.
    SDL_UnlockSurface(surf);
}

// Event loop for displaying a window.
//
// return: nothing.
void event_loop()
{
    // Creates a variable to get the events.
    SDL_Event event;

    // Infinite loop while window is active.
    while (1)
    {
        // Waits for an event.
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            // If the "quit" button is pushed, ends the event loop.
            case SDL_QUIT:
                return;
        }
    }
}

// Creates a window to show a surface.
//
// surf: surface to print on window.
// return: nothing.
void show_surface_on_window(SDL_Surface* surf)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a window, renderer and surface.
    SDL_Window* window = SDL_CreateWindow("Hough Diagram", 0, 0, surf->w, surf->h,SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surf);

    // Copies the texture into the renderer and updates display.
    SDL_RenderCopy(renderer, text, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Dispatches events.
    event_loop();

    // Destroy everything when user exits window.
    SDL_DestroyTexture(text);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void draw_lines_on_window(struct list* list_rho, struct list* list_theta, SDL_Surface* surf, double surf_diag)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a window and initializes parameters.
    int w, h;
    SDL_Window* window = SDL_CreateWindow("Line Detection", 0, 0, 0, 0, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_SetWindowSize(window, w, h);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);

    // Creates two pointers to iterate lists.
    struct list* p_rho = list_rho;
    struct list* p_theta = list_theta;

    // Draw lines.
    while(p_rho && p_theta)
    {
        // Gets values of theta and rho.
        double theta = p_theta->value;
        double rho = p_rho->value;

        // Gets two different points to draw lines.
        double x0 = cos(theta) * rho;
        double y0 = sin(theta) * rho;
        double x1 = x0 + surf_diag * (-sin(theta));
        double y1 = y0 + surf_diag * (cos(theta));
        double x2 = x0 - surf_diag * (-sin(theta));
        double y2 = y0 - surf_diag * (cos(theta));

        // Draws line on renderer.
        SDL_RenderDrawLine(renderer, (int) x1, (int) y1, (int) x2, (int) y2);

        // Goes to the next values.
        p_rho = p_rho->next;
        p_theta = p_theta->next;
    }

    // Updates the display.
    SDL_RenderPresent(renderer);

    // Dispatches events.
    event_loop();

    // Destroy SDL objects.
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}