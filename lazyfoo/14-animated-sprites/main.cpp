#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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
SDL_Renderer* g_renderer = NULL;

class Texture 
{
    public:
        Texture();
        ~Texture();

        void load_from_file(std::string path);
        void free();
        void render(int x, int y, SDL_Rect* clip = NULL);
        int get_width();
        int get_height();
        void set_color(Uint8 r, Uint8 g, Uint8 b);
        void set_blend_mode(SDL_BlendMode blendmode);
        void set_alpha(Uint8 a);

    private:
        SDL_Texture* texture;
        int width;
        int height;
};

Texture::Texture()
{
    texture = NULL;
    width = 0;
    height = 0;
}

Texture::~Texture()
{
    free();
}

void Texture::load_from_file(std::string path)
{
    free();
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == NULL) print_IMG_error_and_quit();

    // Color key image (in that case cyan - 00FFFF)
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF));

    texture = sdlcp(SDL_CreateTextureFromSurface(g_renderer, surface));
    width = surface->w;
    height = surface->h;
    SDL_FreeSurface(surface);
}

void Texture::free()
{
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void Texture::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect render_rect = { .x = x, .y = y, .w = width, .h = height };

    // if we have a clip we need to set the size of the rendered texture to the size of the clip
    if (clip != NULL) {
        render_rect.w = clip->w;
        render_rect.h = clip->h;
    }

    SDL_RenderCopy(g_renderer, texture, clip, &render_rect);
}

int Texture::get_width()
{
    return width;
}

int Texture::get_height()
{
    return height;
}

void Texture::set_color(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(texture, r, g, b);
}

void Texture::set_blend_mode(SDL_BlendMode blendmode)
{
    SDL_SetTextureBlendMode(texture, blendmode);
}

void Texture::set_alpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(texture, alpha);
}

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "Sprites animation", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN));

    //Create vsynced renderer for window
    //We want to use Vertical Sync. VSync allows the rendering to update at the same time as when your monitor updates during vertical refresh.
    g_renderer = sdlcp(
            SDL_CreateRenderer(
                g_window, 
                -1, 
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
            );
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) print_IMG_error_and_quit();
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

    const int SPRITE_FRAMES = 4;
    SDL_Rect sprites[SPRITE_FRAMES];
    Texture spritesheet;

    spritesheet.load_from_file("./sprite.png");

    const int SPRITE_HEIGHT = 205;
    const int SPRITE_WIDTH = 64;

    sprites[0] = {
        .x = 0,
        .y = 0,
        .w = SPRITE_WIDTH,
        .h = SPRITE_HEIGHT,
    };
    sprites[1] = {
        .x = SPRITE_WIDTH,
        .y = 0,
        .w = SPRITE_WIDTH,
        .h = SPRITE_HEIGHT,
    };
    sprites[2] = {
        .x = SPRITE_WIDTH * 2,
        .y = 0,
        .w = SPRITE_WIDTH,
        .h = SPRITE_HEIGHT,
    };
    sprites[3] = {
        .x = SPRITE_WIDTH * 3,
        .y = 0,
        .w = SPRITE_WIDTH,
        .h = SPRITE_HEIGHT,
    };

    bool quit = false;
    SDL_Event ev;
    int frame = 0;

    while (quit == false) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }

        // clear screen
        SDL_SetRenderDrawColor(g_renderer, 0xFF,0xFF,0xFF,0xFF);
        SDL_RenderClear(g_renderer);

        SDL_Rect* current_clip = &sprites[frame / SPRITE_FRAMES];

        spritesheet.render(
                (SCREEN_WIDTH - current_clip->w) / 2,
                (SCREEN_HEIGHT - current_clip->h),
                current_clip);

        SDL_RenderPresent(g_renderer);

        frame += 1;
        if (frame / SPRITE_FRAMES >= SPRITE_FRAMES) {
            frame = 0;
        }
    }

    shutdown();
    return 0;
}

