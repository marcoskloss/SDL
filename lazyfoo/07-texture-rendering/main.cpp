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
SDL_Texture* g_texture = NULL;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "PNG images", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN));

    //https://wiki.libsdl.org/SDL2/SDL_CreateRenderer
    g_renderer = sdlcp(SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED));
    // https://wiki.libsdl.org/SDL2/SDL_SetRenderDrawColor
    // Set the color used for drawing operations (Rect, Line and Clear).
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize PNG loading
    // https://www.libsdl.org/projects/old/SDL_image/docs/SDL_image_8.html
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) print_IMG_error_and_quit();

    g_screen_surface = sdlcp(SDL_GetWindowSurface(g_window));
}

// SDL_Texture: https://wiki.libsdl.org/SDL2/SDL_Texture
SDL_Texture* load_texture(std::string texture_path)
{
    SDL_Surface* loaded_surface = IMG_Load(texture_path.c_str());
    if (loaded_surface == NULL) print_IMG_error_and_quit();

    SDL_Texture* texture = sdlcp(SDL_CreateTextureFromSurface(g_renderer, loaded_surface));
    SDL_FreeSurface(loaded_surface);
    return texture;
}

void load_media()
{
    g_texture = load_texture("background.png");
}

void shutdown()
{
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    init();
    load_media();

    // render texture to screen
    SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
    // update the screen
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

