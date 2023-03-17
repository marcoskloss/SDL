#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
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
SDL_Renderer* g_renderer = NULL; // an SDL_Renderer can render a SDL_Texture to the screen

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "Viewport :D", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN));

    g_renderer = sdlcp(SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED));
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) print_IMG_error_and_quit();

    g_screen_surface = sdlcp(SDL_GetWindowSurface(g_window));
}

void shutdown()
{
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    init();
    SDL_Texture* texture = sdlcp(IMG_LoadTexture(g_renderer, "viewport.png"));

    //Top left corner viewport
    SDL_Rect top_left_viewport;
    top_left_viewport.x = 0;
    top_left_viewport.y = 0;
    top_left_viewport.h = SCREEN_HEIGHT / 2;
    top_left_viewport.w = SCREEN_WIDTH / 2;
    SDL_RenderSetViewport(g_renderer, &top_left_viewport);

    /*
     * Any rendering done after that call will render inside the region defined by 
     * the given viewport. It will also use the coordinate system of the window 
     * it was created in, so the bottom of the view we created will still be y = 480 
     * even though it's only 240 pixels down from the top.
     */
    // render texture to screen
    SDL_RenderCopy(g_renderer, texture, NULL, NULL);

    SDL_Rect top_right_viewport;
    top_right_viewport.x = SCREEN_WIDTH / 2;
    top_right_viewport.y = 0;
    top_right_viewport.h = SCREEN_HEIGHT / 2;
    top_right_viewport.w = SCREEN_WIDTH / 2;
    SDL_RenderSetViewport(g_renderer, &top_right_viewport);
    SDL_RenderCopy(g_renderer, texture, NULL, NULL);

    SDL_Rect bottom_viewport;
    bottom_viewport.x = 0;
    bottom_viewport.y = SCREEN_HEIGHT / 2;
    bottom_viewport.h = SCREEN_HEIGHT / 2;
    bottom_viewport.w = SCREEN_WIDTH;
    SDL_RenderSetViewport(g_renderer, &bottom_viewport);
    SDL_RenderCopy(g_renderer, texture, NULL, NULL);

    SDL_RenderPresent(g_renderer);

    // We can use the IMG_LoadTexture(renderer, img_path)
    // to create textures from images without having to create a temporary surface;
    
    bool quit = false;
    SDL_Event ev;

    while (quit == false) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }
    }

    shutdown();
    return 0;
}

