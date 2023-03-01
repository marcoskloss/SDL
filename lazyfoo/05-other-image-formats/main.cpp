#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
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

SDL_Window* g_window = NULL;
SDL_Surface* g_screen_surface = NULL;
SDL_Surface* g_streched_surface = NULL;

void init()
{
    sdlc(SDL_Init(SDL_INIT_VIDEO));
    g_window = sdlcp(
        SDL_CreateWindow(
            "Splash Screen", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH, 
            SCREEN_HEIGHT, 
            SDL_WINDOW_SHOWN));
    g_screen_surface = sdlcp(SDL_GetWindowSurface(g_window));
}

SDL_Surface* load_surface(std::string image_path)
{
    SDL_Surface* loaded_surface = sdlcp(SDL_LoadBMP(image_path.c_str()));
    // Convert surface to screen format
    /*
        When you load a bitmap, it's typically loaded in a 24bit format since 
        most bitmaps are 24bit. Most, if not all, modern displays are not 24bit by default. 
        If we blit an image that's 24bit onto a 32bit image, 
        SDL will convert it every single time the image is blitted.
    */
    SDL_Surface* optmized_surface = sdlcp(SDL_ConvertSurface(
                loaded_surface, 
                g_screen_surface->format, 
                0));
    // Free unused surface
    SDL_FreeSurface(loaded_surface);
    return optmized_surface;
}

void load_image() 
{
    g_streched_surface = load_surface("splash.bmp");
}

void shutdown()
{
    SDL_FreeSurface(g_streched_surface);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    init();

    load_image();

    // Apply the streched image
    // https://wiki.libsdl.org/SDL2/SDL_Rect
    SDL_Rect strechRect;
    strechRect.x = 0;
    strechRect.y = 0;
    strechRect.w = SCREEN_WIDTH;
    strechRect.h = SCREEN_HEIGHT;
    // SDL_BlitScalled(src, srcrect | NULL, target, rect)
    // with `srcrect` we can crop the src image
    sdlc(SDL_BlitScaled(g_streched_surface, NULL, g_screen_surface, &strechRect));

    // We don't need this anymore
    //sdlc(SDL_BlitSurface(g_streched_surface, NULL, g_screen_surface, NULL));

    SDL_UpdateWindowSurface(g_window);

    SDL_Event e;
    bool quit;  

    while (quit == false) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }       
        }
    }

    shutdown();
    return 0;
}
