#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_SIZE 600
#define CELL_SIZE 20

void* sdlp(void* ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "ERROR: SDL Null pointer found: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = sdlp(SDL_CreateWindow("Snake game", 
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN));
    SDL_Renderer* renderer = sdlp(SDL_CreateRenderer(window, 
                -1, 
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    bool quit = false; 
    SDL_Event ev;
    SDL_Rect head = { .x = 0, .y = 0, .w = CELL_SIZE, .h = CELL_SIZE };

    int direction = SDL_SCANCODE_RIGHT;
    int inputDirection = direction;

    while (!quit) {
        while(SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = true;
        }

        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        if (keyStates[SDL_SCANCODE_UP])
            inputDirection = SDL_SCANCODE_UP;
        if (keyStates[SDL_SCANCODE_DOWN])
            inputDirection = SDL_SCANCODE_DOWN;
        if (keyStates[SDL_SCANCODE_RIGHT])
            inputDirection = SDL_SCANCODE_RIGHT;
        if (keyStates[SDL_SCANCODE_LEFT])
            inputDirection = SDL_SCANCODE_LEFT;

        // this allow the snake movement to follow the grid
        if (direction != inputDirection) {
            bool isVertical = inputDirection == SDL_SCANCODE_UP || inputDirection == SDL_SCANCODE_DOWN;
            bool isHorizontal = inputDirection == SDL_SCANCODE_LEFT || inputDirection == SDL_SCANCODE_RIGHT;
    
            if (isVertical && head.x % CELL_SIZE == 0)
                direction = inputDirection;

            if (isHorizontal && head.y % CELL_SIZE == 0)
                direction = inputDirection;
        }

        if (direction == SDL_SCANCODE_UP)
            head.y -= 1;
        if (direction == SDL_SCANCODE_DOWN)
            head.y += 1;
        if (direction == SDL_SCANCODE_RIGHT)
            head.x += 1;
        if (direction == SDL_SCANCODE_LEFT)
            head.x -= 1;

        if (head.y < 0)
            head.y = SCREEN_SIZE - CELL_SIZE; 
        if (head.y > SCREEN_SIZE - CELL_SIZE)
            head.y = 0;
        if (head.x < 0)
            head.x = SCREEN_SIZE - CELL_SIZE;
        if (head.x > SCREEN_SIZE - CELL_SIZE)
            head.x = 0;

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &head);

        SDL_RenderPresent(renderer);
    }

    return 0;
}
