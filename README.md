# mandelbrot-visualiser

Uses SDL2 to visualise the mandelbrot set

### Build instructions - 
build with the following command: 

>    g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2 -pthread -mavx512f

For displaying frame times include -D_DEBUG

### Usage instructions - 
Use up/down/left/right arrow keys for panning

Use i to zoom in and o to zoom out

Use l for less iterations(less detail) and m for more iterations(more detail)
