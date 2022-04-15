#include "../include/libs.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error!\nUsage: ./benchmark <program_to_benchmark> <style>\n");
        return 1;
    }
    char *prg = argv[1];
    if (strcmp(prg, "sscal") == 0) {
        bench_sscal(atoi(argv[2]));
    } else {
        fprintf(stderr, "Error!\nUnrecognized program\n");
    }
    return 0;
}

void print_stuff(int n, float calcTime, float mem_bw, double gflops, int style, int first) {
    if (style) {
        // table
        if (first)
            printf("Vector size | Execution time | Memory bandwidth | GFLOPS\n");
        printf("%11d | %14f | %16f | %6lf\n", n, calcTime, mem_bw, gflops);
    } else {
        // csv
        printf("%d, %f, %f, %lf\n", n, calcTime * 1000, mem_bw, gflops);
    }
}

void get_bench(int n, float calcTime, int sz, int op, int style, int first) {
    float mem_bw = (float) sz * n * 1e-09 / calcTime;
    double gflops = (float) op * n * 1e-09 / calcTime;
    print_stuff(n, calcTime, mem_bw, gflops, style, first);
}


void bench_sscal(int style) {
    srand((unsigned) time(NULL));
    float randAlpha;
    float *X;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        RANDOM_VEC(X, vec_size, float);
        randAlpha = (float) rand() / RAND_MAX;
        // ****
        CALCTIME(cblas_sscal, vec_size, randAlpha, X, 1);
        // ****
        get_bench(vec_size, calcTime, sizeof(float), 1, style, vec_size == VEC_START);
        vec_size *= 2;
        free(X);
    }
}

