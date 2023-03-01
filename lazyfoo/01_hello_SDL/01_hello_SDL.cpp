//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Check for a possible SDL error code
void sdlc(int code)
{
  if (code < 0) {
    fprintf(stderr, "ERROR (error code found): SDL did something wrong: %s\n", SDL_GetError());
    exit(1);
  }
}

template <typename T>
T* sdlcp(T* ptr) 
{
  if (ptr == NULL) {
    fprintf(stderr, "ERROR (NULL pointer found): SLD did something wrong: %s\n", SDL_GetError());
    exit(1);
  }
  return ptr;
}

int main(int argc, char* args[])
{
	//Initialize SDL
  sdlc(SDL_Init(SDL_INIT_VIDEO));

  // Create window
  SDL_Window* window = sdlcp(
      SDL_CreateWindow(
        "SDL Tutorial", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN)
      );

  //Get window surface
  SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

  //Fill the surface purple
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xCA, 0xA2, 0xF5));
  //Update the surface
  SDL_UpdateWindowSurface(window);
  
  //Hack to get window to stay up
  SDL_Event e; 
  bool quit = false; 

  while(quit == false) { 
    while(SDL_PollEvent(&e)) { 
      if(e.type == SDL_QUIT) quit = true; 
    }
  }

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
