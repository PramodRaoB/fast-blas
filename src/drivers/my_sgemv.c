#include "../../include/libs.h"

void escape(void *p) {
    asm volatile("" : : "g"(p) : "memory");
}

int main(int argc, char *argv[]) {
    srand((unsigned) time(NULL));
    float *X, *Y, *A;
    float alpha = 0.0, beta = 0.0;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        X = (float *) aligned_alloc(32, vec_size * sizeof(float));
        Y = (float *) aligned_alloc(32, vec_size * sizeof(float));
        A = (float *) aligned_alloc(32, vec_size * vec_size * sizeof(float));
        RandomVector(vec_size, X);
        RandomVector(vec_size, Y);
        RandomVector(vec_size * vec_size, A);
        alpha = (float) rand() / RAND_MAX;
        beta = (float) rand() / RAND_MAX;
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = Y[i];
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = X[i];
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            for (int j = 0; j < vec_size; j++) {
                float temp = *(A + i * vec_size + j);
                escape(&temp);
            }
        }
        CALCTIME(cblas_sgemv, CblasRowMajor, CblasNoTrans, vec_size, vec_size, alpha, A, vec_size, X, 1, beta, Y, 1);
        {
            float mem_bw = (float) (4 * vec_size * vec_size + 8 * vec_size) * 1e-09 / calcTime;
            float gflops = (float) (2 * vec_size * vec_size + 3 * vec_size) * 1e-09 / calcTime;
            if (vec_size == VEC_START)
                printf("Vector size | Execution time | Memory bandwidth | GFLOPS\n");
            printf("%11d | %14f | %16f | %6lf\n", vec_size, calcTime, mem_bw, gflops);
        }
        vec_size *= 2;
        free(X);
        free(Y);
        free(A);
    }
    return 0;
}
