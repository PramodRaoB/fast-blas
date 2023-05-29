#include "../../include/libs.h"

void escape(void *p) {
    asm volatile("" : : "g"(p) : "memory");
}

int main(int argc, char *argv[]) {
    srand((unsigned) time(NULL));
    float alpha;
    float *X;
    int vec_size = VEC_START;
    while (vec_size > 0 && vec_size < VEC_LIMIT) {
        alpha = (float) rand() / RAND_MAX;
        RANDOM_VEC(X, vec_size, float);
        // ****
#pragma code_align 32
        for (int i = 0; i < vec_size; i++) {
            float temp = X[i];
            escape(&temp);
        }
        CALCTIME(cblas_sscal, vec_size, alpha, X, 1);
        // ****
//        for (int i = 0; i < vec_size; i++) {
//            escape((float *) bliX.buffer + i);
//        }
//        BLISTIME(bli_scalv, &bliAlpha, &bliX);
        // ****
        vec_size *= 2;
        printf("done\n");
        free(X);
    }
    return 0;
}