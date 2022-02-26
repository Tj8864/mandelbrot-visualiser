/*
    Command to run:

    g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -pthread -mavx512f

    With frame times - 
    
    g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -pthread -mavx512f -D_DEBUG
*/


#include<SDL2/SDL.h>
#include<iostream>
#include"mandelbrot.cpp"
#include"coordinate.h"
#include<thread>
#include<chrono>
#include"sdl_ops.cpp"


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int max_iterations = 256;
int iters[SCREEN_WIDTH][SCREEN_HEIGHT];
coordinate tl = {-2,-1.5}, br = {2,1.5};

SDL_Window* gWindow = nullptr;
SDL_Surface* gScreenSurface = nullptr;
SDL_Surface* gImage = nullptr;
SDL_Renderer* gRenderer = nullptr;


void prepare_image();

int main(int argc, char *argv[])
{
    
    if(!__init())
    {
        std::cout << "Failed to initialize! Aborting\n";
        return 1;
    }

    double differenceX = 4, differenceY = 3;

    SDL_RenderClear(gRenderer);
    updateMandelbrot(0, SCREEN_WIDTH,tl,br);
    prepare_image();
    SDL_RenderPresent(gRenderer);

    bool running = true;
    SDL_Event e;

    while(running) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            }
            if(e.type == SDL_KEYDOWN) {
                bool moved = false;
                if(e.key.keysym.sym == SDLK_o) {
                    coordinate mid = {(tl.x + br.x)/2, (tl.y + br.y)/2};
                    differenceX *= 1.1, differenceY *= 1.1;
                    tl.x = mid.x - differenceX/2;
                    tl.y = mid.y - differenceY/2;
                    br.x = mid.x + differenceX/2;
                    br.y = mid.y + differenceY/2;
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_i) {
                    coordinate mid = {(tl.x + br.x)/2, (tl.y + br.y)/2};
                    differenceX /= 1.1, differenceY /= 1.1;
                    tl.x = mid.x - differenceX/2;
                    tl.y = mid.y - differenceY/2;
                    br.x = mid.x + differenceX/2;
                    br.y = mid.y + differenceY/2;
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_DOWN) {
                    tl.y += (double)differenceY/40;
                    br.y += (double)differenceY/40;   
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_UP) {
                    tl.y -= (double)differenceY/40;
                    br.y -= (double)differenceY/40;  
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_LEFT) {
                    tl.x -= (double)differenceX/40;
                    br.x -= (double)differenceX/40;     
                    moved = true;
                }
                else if(e.key.keysym.sym == SDLK_RIGHT) {
                    tl.x += (double)differenceX/40;
                    br.x += (double)differenceX/40;   
                    moved = true;   
                } else if(e.key.keysym.sym == SDLK_m) {
                    max_iterations += 64;
                    moved = true;
                } else if(e.key.keysym.sym == SDLK_l) {
                    max_iterations -= 64;
                    moved = true;
                }



                if(moved) {
                    auto start_time = std::chrono::high_resolution_clock::now();
                    SDL_RenderClear(gRenderer);
                    std::thread threads[16];
                    int threadWidth = SCREEN_WIDTH/16;
                    for(int i = 0; i < 16; i++) {
                        threads[i] = std::thread(updateMandelbrot, i*threadWidth, (i+1)*threadWidth, tl, br);
                    }
                    for(int i = 0; i < 16; i++) {
                        threads[i].join();
                    }
                    prepare_image();
                    SDL_RenderPresent(gRenderer);
                    auto end_time = std::chrono::high_resolution_clock::now();
                    #ifdef _DEBUG
                    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms\n";
                    #endif
                }
            }
        }
    }

    __close();

    return 0;
}

void prepare_image()
{
    for(int x = 0; x < SCREEN_WIDTH; x++) {
        for(int y = 0; y < SCREEN_HEIGHT; y++) {
            float color = iters[x][y];
            color /= (max_iterations/255.0);
            float r = (3*color+4);
            r -= floor(r/255)*255;
            float g = (5*color+6);
            g -= floor(g/255)*255;
            float b = color/2;
            SDL_SetRenderDrawColor(gRenderer, r, g, b, 255);
            SDL_RenderDrawPoint(gRenderer, x, y);
        }
    }
}

