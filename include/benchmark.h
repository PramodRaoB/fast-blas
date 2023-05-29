#ifndef FAST_BLAS_BENCHMARK_H
#define FAST_BLAS_BENCHMARK_H

#define CALCTIME(f, args...)                                                   \
  struct timeval calc;                                                         \
  tick(&calc);                                                                 \
  f(args);                                                                     \
  double calcTime = tock(&calc);

#define RETCALCTIME(f, args...)                                                   \
  struct timeval calc;                                                         \
  tick(&calc);                                                                 \
  float res = f(args);                                                                     \
  double calcTime = tock(&calc);

#define BLISTIME(f, args...)                                                   \
  struct timeval bliCalc;                                                         \
  tick(&bliCalc);                                                                 \
  f(args);                                                                     \
  double bliCalcTime = tock(&bliCalc);

#define RANDOM_VEC(X, n, type)                                                 \
  X = (type *)aligned_alloc(32, n * sizeof(type));                                        \
  RandomVector(n, X)

#define EPS 1e-2


int VEC_LIMIT = 20000;
//int VEC_LIMIT = 16;
int VEC_START = 64;

void bench_sscal(int style);
void bench_sdot(int style);
void bench_saxpy(int style);
void bench_sgemv(int style);
void print_stuff(int n, float calcTime, float mem_bw, double gflops, int style,
                 int first);
void get_bench(int n, int cnt, float calcTime, int sz, int op, int style, int first);
static void escape(void *p);

#endif // FAST_BLAS_BENCHMARK_H
