#include "../include/cblas.h"
#include <math.h>
#include <stdlib.h>

void cblas_sscal(const int N, const float alpha, float *X, const int incX) {
    if (fabs(alpha - 0.0) < 1e-6) {
#pragma omp parallel for num_threads(8)
        for (int i = 0; i < N; i++) X[i * incX] = 0.0;
        return;
    }
#pragma omp parallel for num_threads(8)
//#pragma omp simd
    for (int i = 0; i < N; i++) {
        X[i * incX] *= alpha;
    }
}

float cblas_sdot(const int N, const float *X, const int incX, const float *Y,
                 const int incY) {
    float res = 0.0;
#pragma omp parallel for reduction(+:res) num_threads(8)
//#pragma omp simd
    for (int i = 0; i < N; i++) {
        res += X[i * incX] * Y[i * incY];
    }
    return res;
}

void cblas_saxpy(const int N, const float alpha, const float *X, const int incX, float *Y, const int incY) {
//#pragma omp simd
//#pragma omp parallel for
#pragma omp parallel
    {
#pragma omp for simd
        for (int i = 0; i < N; i++) {
            Y[i * incY] += alpha * X[i * incX];
        }
    }
}

void cblas_sgemv(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const float alpha, const float *A, const int lda, const float *X, const int incX, const float beta,
                 float *Y, const int incY) {
    int lenX = N, lenY = M;
    if (TransA == CblasTrans) {
        lenX = M, lenY = N;
    }
    cblas_sscal(lenY, beta, Y, incY);
    if ((order & 1) ^ (TransA & 1)) {
#pragma omp parallel num_threads(8)
        {
            float *privateY = (float *) calloc(lenY, sizeof(float));
#pragma omp for
            for (int i = 0; i < lenX; i++) {
//                cblas_saxpy(lenY, alpha * X[i * incX], A + i * lda, 1, privateY, incY);
#pragma omp simd
                    for (int j = 0; j < lenY; j++) {
                        privateY[j * incY] += alpha * X[i * incX] * A[i * lda + j];
                    }
            }
#pragma omp critical
            {
//                cblas_saxpy(lenY, 1.0, privateY, incY, Y, incY);
#pragma omp simd
                    for (int j = 0; j < N; j++) {
                        Y[j * incY] += privateY[j * incY];
                    }
            };
        }
    }
    else {
        //dot
#pragma omp parallel for num_threads(8)
        for (int i = 0; i < lenY; i++) {
//            Y[i * incY] += alpha * cblas_sdot(lenX, A + i * lda, 1, X, incX);
            float res = 0.0;
//#pragma omp parallel for reduction(+:res) num_threads(2)
            for (int j = 0; j < lenX; j++) {
                res += A[i * lda + j] * X[j * incX];
            }
            Y[i * incY] += alpha * res;
        }
    }
}

