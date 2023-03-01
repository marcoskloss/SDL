#include <SDL2/SDL.h>
#include <stdio.h>

enum KeyPressSurfaces {
    KEY_PRESS_SURFACE_DEFAULT = 0,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
};

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

SDL_Window* g_window = NULL;
SDL_Surface* g_screen_surface = NULL;
SDL_Surface* g_current_surface = NULL;
const char* g_keypress_surfaces[] = {"splash.bmp", "arrowup.bmp", "arrowdown.bmp", "arrowleft.bmp", "arrowright.bmp"};

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

void load_image(KeyPressSurfaces index)
{
    const char* image_path = g_keypress_surfaces[index];
    g_current_surface = sdlcp(SDL_LoadBMP(image_path));
}

void shutdown()
{
    SDL_FreeSurface(g_current_surface);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    init();

    load_image(KEY_PRESS_SURFACE_DEFAULT);
    SDL_BlitSurface(g_current_surface, NULL, g_screen_surface, NULL);
    SDL_UpdateWindowSurface(g_window);

    SDL_Event e;
    bool quit;  

    while (quit == false) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }       
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    load_image(KEY_PRESS_SURFACE_UP);
                    break;
                case SDLK_DOWN:
                    load_image(KEY_PRESS_SURFACE_DOWN);
                    break;
                case SDLK_LEFT:
                    load_image(KEY_PRESS_SURFACE_LEFT);
                    break;
                case SDLK_RIGHT:
                    load_image(KEY_PRESS_SURFACE_RIGHT);
                    break;
                default:
                    load_image(KEY_PRESS_SURFACE_DEFAULT);
                    break;
                }
                // apply the image
                SDL_BlitSurface(g_current_surface, NULL, g_screen_surface, NULL);
                SDL_UpdateWindowSurface(g_window);
            }
        }
    }

    shutdown();
    return 0;
}
