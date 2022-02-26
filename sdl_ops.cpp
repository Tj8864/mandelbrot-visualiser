#include<SDL2/SDL.h>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window* gWindow;
extern SDL_Surface* gScreenSurface;
extern SDL_Surface* gImage;
extern SDL_Renderer* gRenderer;

bool __init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL Init failed, aborting!\n";
        return 0;
    }
    gWindow = SDL_CreateWindow("Mandelbrot set visualiser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(gWindow == nullptr){
        std::cerr << "SDL Window creation failed, aborting!\n";
        return 0;
    }
    gScreenSurface = SDL_GetWindowSurface(gWindow);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if(gRenderer == nullptr){
        std::cerr << "SDL Renderer creation failed, aborting!\n";
        return 0;
    }
    SDL_SetRenderDrawColor(gRenderer, 0x0, 0xFF, 0xFF, 0xFF);

    return true;
}

bool __close() {
    std::cout << "Close called\n";
    SDL_FreeSurface(gImage);
    gImage = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    SDL_Quit();
    return true;
}

