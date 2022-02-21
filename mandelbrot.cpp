#include<complex>
#include"coordinate.h"

extern const int max_iterations;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

int count_iterations(int pixel_x, int pixel_y, coordinate top_left, coordinate bottom_right) {
    double x_coord, y_coord;
    x_coord = (double)pixel_x/SCREEN_WIDTH * (bottom_right.x - top_left.x) + top_left.x;
    y_coord = (double)pixel_y/SCREEN_HEIGHT * (bottom_right.y - top_left.y) + top_left.y;
    std::complex<double> c(x_coord, y_coord);
    std::complex<double> z(0,0);
    if(c.real()*c.real() + c.imag()*c.imag() > 4) {
        return 0;
    }
    
    for(int _ = 0; _ < max_iterations; _++) {
        z = z*z + c;
        if(z.real()*z.real() + z.imag()*z.imag() > 4) {
            return _;
        }
    }
    
    return max_iterations;
}