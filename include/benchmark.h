#ifndef FAST_BLAS_BENCHMARK_H
#define FAST_BLAS_BENCHMARK_H

#define CALCTIME(f, args...)                                                   \
  struct timeval calc;                                                         \
  tick(&calc);                                                                 \
  f(args);                                                                     \
  double calcTime = tock(&calc);

#define RANDOM_VEC(X, n, type)                                                 \
  X = (type *)malloc(n * sizeof(type));                                        \
  RandomVector(n, X)
int VEC_LIMIT = 1000000000;
int VEC_START = 10000;

void bench_sscal(int style);
void print_stuff(int n, float calcTime, float mem_bw, double gflops, int style,
                 int first);
void get_bench(int n, float calcTime, int sz, int op, int style, int first);

#endif // FAST_BLAS_BENCHMARK_H
