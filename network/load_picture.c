#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>

// Event loop that calls the relevant event handler.

// path: Path of the image.

SDL_Surface* load_image(const char* path) {

    // add check for problem and return error
    SDL_Surface* surfacet = IMG_Load(path);
    if (surfacet == NULL) {
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    }
    SDL_Surface* surface
        = SDL_ConvertSurfaceFormat(surfacet, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surfacet);
    return surface;
}

void surface_to_grayscale(SDL_Surface* surface, double* vector) {
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++) {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], format, &r, &g, &b);
        *(vector + i) = r / 128.0;
    }
    SDL_UnlockSurface(surface);
}

double* vector_from_image(char* path) {
    SDL_Surface* surface = load_image(path);
    double* vector = malloc(sizeof(double) * surface->w * surface->h);
    // int w = surface->w , h = surface->h;
    surface_to_grayscale(surface, vector);

    SDL_FreeSurface(surface);
    return vector;
}
