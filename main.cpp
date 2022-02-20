#include<SDL2/SDL.h>
#include<iostream>
#include"mandelbrot.cpp"
#include"coordinate.h"
#include<thread>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int max_iterations = 255;

SDL_Window* gWindow = nullptr;
SDL_Surface* gScreenSurface = nullptr;
SDL_Surface* gImage = nullptr;
SDL_Renderer* gRenderer = nullptr;

bool __init();
bool __loadMedia(const char * file);
bool __close();

coordinate tl = {-2,-2}, br = {2,2};

int updateMandelbrot(int start_x, int end_x)
{
    for(int x = start_x; x < end_x; x++) {
        for(int y = 0; y < SCREEN_HEIGHT; y++) {
            int _iters = count_iterations(x,y,tl,br);
            Uint8 color = (Uint8)_iters;
            SDL_SetRenderDrawColor(gRenderer, color, color, color, 255);
            SDL_RenderDrawPoint(gRenderer, x, y);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    
    if(!__init())
    {
        std::cout << "Failed to initialize! Aborting\n";
        return 1;
    }

    double difference = 4;

    SDL_RenderClear(gRenderer);
    bool running = true;
    SDL_Event e;
    while(running) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            }
            if(e.type == SDL_KEYDOWN) {
                bool moved = false;
                if(e.key.keysym.sym == SDLK_x) {
                    coordinate mid = {(tl.x + br.x)/2, (tl.y + br.y)/2};
                    difference /= 1.1;
                    tl.x = mid.x - difference/2;
                    tl.y = mid.y - difference/2;
                    br.x = mid.x + difference/2;
                    br.y = mid.y + difference/2;
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_z) {
                    coordinate mid = {(tl.x + br.x)/2, (tl.y + br.y)/2};
                    std::cout << tl.x << " " << tl.y << std::endl << br.x << " " << br.y << std::endl;
                    difference *= 1.1;
                    tl.x = mid.x - difference/2;
                    tl.y = mid.y - difference/2;
                    br.x = mid.x + difference/2;
                    br.y = mid.y + difference/2;
                    std::cout << tl.x << " " << tl.y << std::endl << br.x << " " << br.y << std::endl;
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_UP) {
                    tl.y += (double)difference/40;
                    br.y += (double)difference/40;   
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_DOWN) {
                    tl.y -= (double)difference/40;
                    br.y -= (double)difference/40;  
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_LEFT) {
                    tl.x -= (double)difference/40;
                    br.x -= (double)difference/40;     
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_RIGHT) {
                    tl.x += (double)difference/40;
                    br.x += (double)difference/40;   
                    moved = true;   
                }

                if(moved) {
                    std::thread threads[16];
                    int thread_width = (SCREEN_WIDTH)/16;
                    SDL_RenderClear(gRenderer);
                    for(int i=0;i<16;i++) {
                        std::cout << "Thread " << i+1 << " created\n";
                        threads[i] = std::thread(updateMandelbrot, i*thread_width, (i+1)*thread_width);
                    }
                    for(auto &t:threads) {
                        t.join();
                        std::cout << "Thread joined\n";
                    }
                    SDL_RenderPresent(gRenderer);
                }
            }
        }
    }

    __close();

    return 0;
}

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

bool __loadMedia(const char * file) {
    gImage = SDL_LoadBMP(file);
    if(gImage == nullptr) {
        std::cerr << "Failed to load image, aborting!\n";
        return 0;
    }
    SDL_BlitSurface(gImage, nullptr, gScreenSurface, nullptr);
    SDL_UpdateWindowSurface(gWindow);
    return true;
}

bool __close() {
    SDL_FreeSurface(gImage);
    gImage = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    SDL_Quit();
    return true;
}

