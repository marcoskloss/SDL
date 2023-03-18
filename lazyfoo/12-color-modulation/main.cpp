#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
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

Texture texture;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "Clip rendering", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN));

    g_renderer = sdlcp(SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED));
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

    texture.load_from_file("./preview.png");

    bool quit = false;
    SDL_Event ev;

    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;

    Uint8 offset = 32;

    while (quit == false) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;

            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                //Increase red
                    case SDLK_q:
                    r += 32;
                    break;
                    
                    //Increase green
                    case SDLK_w:
                    g += 32;
                    break;
                    
                    //Increase blue
                    case SDLK_e:
                    b += 32;
                    break;
                    
                    //Decrease red
                    case SDLK_a:
                    r -= 32;
                    break;
                    
                    //Decrease green
                    case SDLK_s:
                    g -= 32;
                    break;
                    
                    //Decrease blue
                    case SDLK_d:
                    b -= 32;
                    break;

                    case SDLK_r:
                    r = 255;
                    g = 255;
                    b = 255;
                    break;
                }
            }
        }

        // clear screen
        SDL_SetRenderDrawColor(g_renderer, 0xFF,0xFF,0xFF,0xFF);
        SDL_RenderClear(g_renderer);

        texture.set_color(r, g, b);
        texture.render(0, 0);
        
        SDL_RenderPresent(g_renderer);
    }

    shutdown();
    return 0;
}

