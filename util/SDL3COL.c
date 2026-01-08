#include <SDL2/SDL.h>

void drawColumn(SDL_Renderer *r, float x, float y1, float y2) {
    SDL_RenderLine(r, x, y1, x, y2);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Columns", 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL, 0);
    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = 0;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (float x = 50; x < 640; x += 50) {
            drawColumn(renderer, x, 100, 380);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
