#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

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

template <typename T>
T* ttfcp(T* ptr) 
{
    if (ptr == NULL) {
        fprintf(stderr, "ERROR (NULL pointer found): SDL_ttf: %s\n", TTF_GetError());
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

        #if defined(SDL_TTF_MAJOR_VERSION)
        void load_from_rendered_text(std::string text, SDL_Color color, TTF_Font* font);
        #endif
        void render(
                    int x, int y,
                    SDL_Rect* clip = NULL,
                    double angle = 0.0,
                    SDL_Point* center = NULL,
                    SDL_RendererFlip flip = SDL_FLIP_NONE
                );
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

void Texture::render(
        int x, int y,
        SDL_Rect* clip,
        double angle,
        SDL_Point* center,
        SDL_RendererFlip flip)
{
    SDL_Rect render_rect = { .x = x, .y = y, .w = width, .h = height };

    // if we have a clip we need to set the size of the rendered texture to the size of the clip
    if (clip != NULL) {
        render_rect.w = clip->w;
        render_rect.h = clip->h;
    }

    SDL_RenderCopyEx(g_renderer, texture, clip, &render_rect, angle, center, flip);
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

// There are other ways to render text that are smoother or blended.
// Shaded/Blended rendering might work better for different text sizes.

#if defined(SDL_TTF_MAJOR_VERSION)
void Texture::load_from_rendered_text(std::string text, SDL_Color color, TTF_Font* font)
{
    free();

    SDL_Surface* text_surface = sdlcp(TTF_RenderText_Solid(font, text.c_str(), color));
    texture = sdlcp(SDL_CreateTextureFromSurface(g_renderer, text_surface));
    width = text_surface->w;
    height = text_surface->h;
    SDL_FreeSurface(text_surface);
}
#endif

Texture g_texture;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(SDL_CreateWindow(
            "Timing!",
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
    
    if (TTF_Init() == -1) {
        printf("ERROR: SDL_ttf could not initialize: %s/n", TTF_GetError());
        exit(1);
    }
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

    Texture prompt_text;
    Texture time_texture;

    SDL_Color black_text_color = { 0xFF, 0xFF, 0xFF, 0xFF};
                                                      
    TTF_Font* font = ttfcp(TTF_OpenFont("./Ubuntu-Th.ttf", 28));
    prompt_text.load_from_rendered_text(
            "Press enter to reset start time.", black_text_color, font);

    bool quit = false;
    SDL_Event ev;

    // Current time start time
    Uint32 start_time = 0;

    // In memory text stream
    std::stringstream time_text;

    while (!quit) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN) {
                start_time = SDL_GetTicks(); // returns the time since the program started
            }
        }

        time_text.str("");
        time_text << "Milliseconds since start time: " << SDL_GetTicks() - start_time;
        time_texture.load_from_rendered_text(time_text.str().c_str(), black_text_color, font);

        // clear screen
        SDL_SetRenderDrawColor(g_renderer, 0x00,0x00,0x00,0xFF);
        SDL_RenderClear(g_renderer);

        prompt_text.render(20, 20);
        time_texture.render(20, 20 + prompt_text.get_height());

        SDL_RenderPresent(g_renderer);
    }

    shutdown();
    return 0;
}
