#include "../include/libs.h"
#include <blis/blis.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error!\nUsage: ./benchmark <program_to_benchmark> <style>\n");
        return 1;
    }
    char *prg = argv[1];
    if (strcmp(prg, "sscal") == 0) {
        bench_sscal(atoi(argv[2]));
    } else if (strcmp(prg, "sdot") == 0) {
        bench_sdot(atoi(argv[2]));
    } else if (strcmp(prg, "saxpy") == 0) {
        bench_saxpy(atoi(argv[2]));
    } else if (strcmp(prg, "sgemv") == 0) {
        bench_sgemv(atoi(argv[2]));
    } else {
        fprintf(stderr, "Error!\nUnrecognized program\n");
    }
    return 0;
}

int sverify(void *a, void *b, int n) {
    float *fa = (float *) a;
    float *fb = (float *) b;
    for (int i = 0; i < n; i++) {
        if (fabs(fa[i] - fb[i]) > EPS) return 0;
    }
    return 1;
}

void print_stuff(int n, float calcTime, float mem_bw, double gflops, int style, int first) {
    if (style) {
        // table
        if (first)
            printf("Vector size | Execution time | Memory bandwidth | GFLOPS\n");
        printf("%11d | %14f | %16f | %6lf\n", n, calcTime, mem_bw, gflops);
    } else {
        // csv
        if (first)
            printf("Vector size,Execution time,Memory bandwidth,GFLOPS\n");
        printf("%d,%f,%f,%lf\n", n, calcTime * 1000, mem_bw, gflops);
    }
}

void get_bench(int n, int cnt, float calcTime, int sz, int op, int style, int first) {
    float mem_bw = (float) sz * n * cnt * 1e-09 / calcTime;
    double gflops = (float) op * n * 1e-09 / calcTime;
    print_stuff(n, calcTime, mem_bw, gflops, style, first);
}

void escape(void *p) {
    asm volatile("" : : "g"(p) : "memory");
}

void bench_sscal(int style) {
    srand((unsigned) time(NULL));
    float alpha;
    float *X;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        alpha = (float) rand() / RAND_MAX;
        obj_t bliX, bliAlpha;
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliX);
        bli_randm(&bliX);
        X = (float *) aligned_alloc(32, vec_size * sizeof(float));
        memcpy(X, bliX.buffer, sizeof(float) * vec_size);
        bli_obj_create_1x1(BLIS_FLOAT, &bliAlpha);
        bli_setsc(alpha, 0.0, &bliAlpha);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = X[i];
            escape(&temp);
        }
        CALCTIME(cblas_sscal, vec_size, alpha, X, 1);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliX.buffer + i);
            escape(&temp);
        }
        BLISTIME(bli_scalv, &bliAlpha, &bliX);
        // ****
        assert(sverify(X, bliX.buffer, vec_size));
        get_bench(vec_size, 1, calcTime, sizeof(float), 1, style, vec_size == VEC_START);
//        get_bench(vec_size, 1, bliCalcTime, sizeof(float), 1, style, 0);
        vec_size *= 2;
        free(X);
        bli_obj_free(&bliX);
        bli_obj_free(&bliAlpha);
    }
}

void bench_sdot(int style) {
    srand((unsigned) time(NULL));
    float *X, *Y;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        obj_t bliX, bliY;
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliX);
        bli_randm(&bliX);
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliY);
        bli_randm(&bliY);
        X = (float *) aligned_alloc(32, vec_size * sizeof(float));
        Y = (float *) aligned_alloc(32, vec_size * sizeof(float));
        memcpy(X, bliX.buffer, sizeof(float) * vec_size);
        memcpy(Y, bliY.buffer, sizeof(float) * vec_size);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = X[i];
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = Y[i];
            escape(&temp);
        }
        RETCALCTIME(cblas_sdot, vec_size, X, 1, Y, 1);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliX.buffer + i);
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliY.buffer + i);
            escape(&temp);
        }
        obj_t rho;
        bli_obj_create_1x1(BLIS_FLOAT, &rho);
        bli_setsc(0.0, 0.0, &rho);
        BLISTIME(bli_dotv, &bliX, &bliY, &rho);
        // ****
        assert(sverify(&res, rho.buffer, 1));
        get_bench(vec_size, 2, calcTime, sizeof(float), 2, style, vec_size == VEC_START);
//        get_bench(vec_size, 2, bliCalcTime, sizeof(float), 2, style, 0);
        vec_size *= 2;
        free(X);
        free(Y);
        bli_obj_free(&bliX);
        bli_obj_free(&bliY);
        bli_obj_free(&rho);
    }
}

void bench_saxpy(int style) {
    srand((unsigned) time(NULL));
    float *X, *Y;
    float alpha = 0.0;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        obj_t bliX, bliY, bliAlpha;
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliX);
        bli_randm(&bliX);
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliY);
        bli_randm(&bliY);
        bli_obj_create_1x1(BLIS_FLOAT, &bliAlpha);
        bli_randm(&bliAlpha);
        X = (float *) aligned_alloc(32, vec_size * sizeof(float));
        Y = (float *) aligned_alloc(32, vec_size * sizeof(float));
        memcpy(X, bliX.buffer, sizeof(float) * vec_size);
        memcpy(Y, bliY.buffer, sizeof(float) * vec_size);
        alpha = *(float *) bliAlpha.buffer;
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = X[i];
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = Y[i];
            escape(&temp);
        }
        CALCTIME(cblas_saxpy, vec_size, alpha, X, 1, Y, 1);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliX.buffer + i);
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliY.buffer + i);
            escape(&temp);
        }
        BLISTIME(bli_axpyv, &bliAlpha, &bliX, &bliY);
        // ****
        assert(sverify(bliY.buffer, Y, vec_size));
        get_bench(vec_size, 2, calcTime, sizeof(float), 2, style, vec_size == VEC_START);
        get_bench(vec_size, 2, bliCalcTime, sizeof(float), 2, style, 0);
        vec_size *= 2;
        free(X);
        free(Y);
        bli_obj_free(&bliX);
        bli_obj_free(&bliY);
        bli_obj_free(&bliAlpha);
    }
}

void bench_sgemv(int style) {
    srand((unsigned) time(NULL));
    float *X, *Y, *A;
    float alpha = 0.0, beta = 0.0;
    int vec_size = VEC_START;
    int trans = 0;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        obj_t bliX, bliY, bliAlpha, bliBeta, bliA;
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliX);
        bli_randm(&bliX);
        bli_obj_create(BLIS_FLOAT, 1, vec_size, vec_size, 1, &bliY);
        bli_randm(&bliY);
        bli_obj_create_1x1(BLIS_FLOAT, &bliAlpha);
        bli_randm(&bliAlpha);
        bli_obj_create_1x1(BLIS_FLOAT, &bliBeta);
        bli_randm(&bliBeta);
        bli_obj_create(BLIS_FLOAT, vec_size, vec_size, vec_size, 1, &bliA);
        bli_randm(&bliA);
        X = (float *) aligned_alloc(32, vec_size * sizeof(float));
        Y = (float *) aligned_alloc(32, vec_size * sizeof(float));
        A = (float *) aligned_alloc(32, vec_size * vec_size * sizeof(float));
        memcpy(X, bliX.buffer, sizeof(float) * vec_size);
        memcpy(Y, bliY.buffer, sizeof(float) * vec_size);
        memcpy(A, bliA.buffer, sizeof(float) * vec_size * vec_size);
        alpha = *(float *) bliAlpha.buffer;
        beta = *(float *) bliBeta.buffer;
        if (trans)
            bli_obj_induce_trans(&bliA);
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
        CALCTIME(cblas_sgemv, CblasRowMajor, trans ? CblasTrans : CblasNoTrans, vec_size, vec_size, alpha, A, vec_size, X, 1, beta, Y, 1);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliY.buffer + i);
            escape(&temp);
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            for (int j = 0; j < vec_size; j++) {
                float temp = *((float *) bliA.buffer + i * vec_size + j);
                escape(&temp);
            }
        }
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = *((float *) bliX.buffer + i);
            escape(&temp);
        }
        BLISTIME(bli_gemv, &bliAlpha, &bliA, &bliX, &bliBeta, &bliY);
        // ****
        assert(sverify(bliY.buffer, Y, vec_size));
        if (trans) {
            {
                float mem_bw = (float) (4 * vec_size * vec_size + 8 * vec_size) * 1e-09 / calcTime;
                float gflops = (float) (2 * vec_size * vec_size + 3 * vec_size) * 1e-09 / calcTime;
                print_stuff(vec_size, calcTime, mem_bw, gflops, style, vec_size == VEC_START);
            }
            {
                float mem_bw = (float) (4 * vec_size * vec_size + 8 * vec_size) * 1e-09 / bliCalcTime;
                float gflops = (float) (2 * vec_size * vec_size + 3 * vec_size) * 1e-09 / bliCalcTime;
                print_stuff(vec_size, bliCalcTime, mem_bw, gflops, style, false);
            }
        }
        if (trans)
            vec_size *= 2;
        trans ^= 1;
        free(X);
        free(Y);
        free(A);
        bli_obj_free(&bliX);
        bli_obj_free(&bliY);
        bli_obj_free(&bliAlpha);
        bli_obj_free(&bliA);
        bli_obj_free(&bliBeta);
    }
}
