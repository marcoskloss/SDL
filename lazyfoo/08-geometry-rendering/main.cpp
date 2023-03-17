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

    g_renderer = sdlcp(SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED));
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
    
    bool quit = false;
    SDL_Event ev;


    while (quit == false) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }

        SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(g_renderer);

        // fill rect
        const int fill_rect_width = SCREEN_WIDTH / 4;
        const int fill_rect_height = SCREEN_HEIGHT / 4;
        const int fill_rect_x = (SCREEN_WIDTH/2) - (fill_rect_width/2);
        const int fill_rect_y = (SCREEN_HEIGHT/2) - (fill_rect_height/2);

        SDL_Rect fill_rect = {
            .x = fill_rect_x,
            .y = fill_rect_y,
            .w = fill_rect_width,
            .h = fill_rect_height,
        };
        SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(g_renderer, &fill_rect);

        // outline rect
        const int outline_rect_width = fill_rect_width * 2;
        const int outline_rect_height = fill_rect_height * 2;
        const int outline_rect_x = (SCREEN_WIDTH/2) - (outline_rect_width/2);
        const int outline_rect_y = (SCREEN_HEIGHT/2) - (outline_rect_height/2);

        SDL_Rect outline_rect = {
            .x = outline_rect_x,
            .y = outline_rect_y,
            .w = outline_rect_width,
            .h = outline_rect_height,
        };
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(g_renderer, &outline_rect);

        // line
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawLine(g_renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // dotted line
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0xFF);
        for (int y = fill_rect_y; y < fill_rect_height + fill_rect_y; y += 4) {
            SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH/2, y);
        }

        SDL_RenderPresent(g_renderer);
    }

    shutdown();
    return 0;
}

