#include <SDL2/SDL.h>
#include <stdio.h>

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
        fprintf(stderr, "ERROR (NULL pointer found): SLD did something wrong: %sn", SDL_GetError());
        exit(1);
    }
      return ptr;
}

SDL_Window* g_window = NULL;
SDL_Surface* g_screen_surface = NULL;
SDL_Surface* g_hello_world = NULL;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(
        SDL_CreateWindow(
            "Splash Screen", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            800, 
            600, 
            SDL_WINDOW_SHOWN));
    g_screen_surface = sdlcp(SDL_GetWindowSurface(g_window));
}

void load_media()
{
    g_hello_world = sdlcp(SDL_LoadBMP("splash.bmp"));
}

void shutdown()
{
    SDL_FreeSurface(g_hello_world);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    init();
    load_media();

    SDL_Event e;
    bool quit;
    
    // apply the image
    SDL_BlitSurface(g_hello_world, NULL, g_screen_surface, NULL);
    SDL_UpdateWindowSurface(g_window);

    while (quit == false) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
    }

    shutdown();
    return 0;
}
