#include "../../include/libs.h"

#define VEC_LIMIT 1000000000

int main(int argc, char *argv[]) {
    struct timeval calc;
    double calcTime;
    double gflops;
    float mem_bw;
    float randAlpha;
    float *X;
    int vec_size = 5000;
    srand((unsigned) time(NULL));
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        X = (float *) malloc(vec_size * sizeof(float));
        gflops = 1.0 * vec_size * 1e-09;
        randAlpha = (float) rand() / RAND_MAX;
        RandomVector(vec_size, X);
        // ****
        tick(&calc);
        cblas_sscal(vec_size, randAlpha, X, 1);
        calcTime = tock(&calc);
        // ****
        mem_bw = 4.0 * vec_size * 1e-09 / calcTime;
        vec_size *= 2;
        free(X);
    }
}