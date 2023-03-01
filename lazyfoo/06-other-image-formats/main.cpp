#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Check for a possible SDL error code
void sdlc(int code)
{
    if (code < 0) {
        fprintf(stderr, "ERROR (error code found): SDL did something wrong: %s\n", SDL_GetError());
        exit(1);
    }
}

// Check for a possible NULL pointer
template <typename T>
T* sdlcp(T* ptr) 
{
    if (ptr == NULL) {
        fprintf(stderr, "ERROR (NULL pointer found): SLD did something wrong: %s\n", SDL_GetError());
        exit(1);
    }
      return ptr;
}

void print_IMG_error_and_quit()
{
    printf("ERROR (SDL_image error): %s\n", IMG_GetError());
    exit(1);
}

SDL_Window* g_window = NULL;
SDL_Surface* g_screen_surface = NULL;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "PNG images", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN));

    // Initialize PNG loading
    // https://www.libsdl.org/projects/old/SDL_image/docs/SDL_image_8.html
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) print_IMG_error_and_quit();

    g_screen_surface = sdlcp(SDL_GetWindowSurface(g_window));
}

SDL_Surface* load_surface(std::string image_path)
{
    SDL_Surface* loaded_surface = IMG_Load(image_path.c_str());
    if (loaded_surface == NULL) print_IMG_error_and_quit();

    SDL_Surface* optmized_surface = sdlcp(SDL_ConvertSurface(
                loaded_surface, 
                g_screen_surface->format, 
                0));

    SDL_FreeSurface(loaded_surface);
    return optmized_surface;
}

void shutdown()
{
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    init();
    SDL_Surface* img_surface = load_surface("background.png");
    
    SDL_Rect strechRect;
    strechRect.x = 0;
    strechRect.y = 0;
    strechRect.w = SCREEN_WIDTH;
    strechRect.h = SCREEN_HEIGHT;

    sdlc(SDL_BlitScaled(img_surface, NULL, g_screen_surface, &strechRect));

    SDL_UpdateWindowSurface(g_window);

    bool quit = false;
    SDL_Event ev;

    while (quit == false) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }
    }

    return 0;
}
