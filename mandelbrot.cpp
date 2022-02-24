#include<complex>
#include"coordinate.h"
#include<immintrin.h>

extern int max_iterations;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern int iters[640][480];

#define wide_double __m256d
#define wide_int __m256i
#define multiplyD(a,b) _mm256_mul_pd(a,b)
#define multiplyI(a,b) _mm256_mul_epi64(a,b)
#define addD(a,b) _mm256_add_pd(a,b)
#define addI(a,b) _mm256_add_epi64(a,b)
#define subD(a,b) _mm256_sub_pd(a,b)
#define subI(a,b) _mm256_sub_epi64(a,b)
#define andI(a,b) _mm256_and_si256(a,b)


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

double pixel_to_coord_X(int pixel_x, coordinate top_left, coordinate bottom_right) {
    return (double)pixel_x/SCREEN_WIDTH * (bottom_right.x - top_left.x) + top_left.x;
}

double pixel_to_coord_Y(int pixel_y, coordinate top_left, coordinate bottom_right) {
    return (double)pixel_y/SCREEN_HEIGHT * (bottom_right.y - top_left.y) + top_left.y;
}
#define p2cx(x) pixel_to_coord_X(x, top_left, bottom_right)
#define p2cy(y) pixel_to_coord_Y(y, top_left, bottom_right)

int updateMandelbrot(int from, int to, coordinate top_left, coordinate bottom_right) {
    for(int x=from; x<to; x++) {
        for(int y=0; y<SCREEN_HEIGHT; y+=4) {
            wide_double z_real, z_imag, z_real_sq, z_imag_sq, c_real, c_imag, res_real, res_imag, two, four, mask1;
            wide_int iterations, one, mask2, _max_iterations;
            c_real = _mm256_set_pd(p2cx(x), p2cx(x), p2cx(x), p2cx(x));
            c_imag = _mm256_set_pd(p2cy(y+3), p2cy(y+2), p2cy(y+1), p2cy(y));
            z_real = _mm256_setzero_pd();
            z_imag = _mm256_setzero_pd();
            two = _mm256_set1_pd(2);
            four = _mm256_set1_pd(4);
            one = _mm256_set1_epi64x(1);
            mask2 = _mm256_set1_epi64x(1);
            iterations = _mm256_set1_epi64x(0);
            _max_iterations = _mm256_set1_epi64x(max_iterations);

            loop:
                z_real_sq = multiplyD(z_real, z_real);
                z_imag_sq = multiplyD(z_imag, z_imag);
                res_real = subD(z_real_sq, z_imag_sq);
                res_real = addD(res_real, c_real);
                res_imag = multiplyD(multiplyD(two, z_real), z_imag);
                res_imag = addD(res_imag, c_imag);
                z_real = res_real;
                z_imag = res_imag;
                mask1 = _mm256_cmp_pd(addD(z_real_sq,z_imag_sq), four, _CMP_LT_OQ);
                mask2 = _mm256_cmpgt_epi64(_max_iterations,iterations);
                mask2 = andI(mask2,_mm256_castpd_si256(mask1));
                iterations = addI(iterations, andI(mask2, one));
                if(_mm256_movemask_pd(_mm256_castsi256_pd(mask2)) > 0) {
                    // std::cout << "Loop repeated" << std::endl;
                    goto loop;
                }
            
            iters[x][y] = _mm256_extract_epi64(iterations,0);
            iters[x][y+1] = _mm256_extract_epi64(iterations,1);
            iters[x][y+2] = _mm256_extract_epi64(iterations,2);
            iters[x][y+3] = _mm256_extract_epi64(iterations,3);
            // std::cout << iters[x][y] << " ";
        }
    }
    return 0;
}