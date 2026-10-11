#define _POSIX_C_SOURCE 200809L
#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include <stdio.h>
#include <time.h>

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static void bench_gemm(int n, int reps) {
    mat_f a = mat_alloc((size_t)n, (size_t)n);
    mat_f b = mat_alloc((size_t)n, (size_t)n);
    mat_f c = mat_alloc((size_t)n, (size_t)n);
    int r;
    double t0, t1, flops;
    mat_fill(&a, 0.1f);
    mat_fill(&b, 0.2f);
    mat_gemm(1.0f, &a, &b, 0.0f, &c);
    t0 = now_s();
    for (r = 0; r < reps; ++r) {
        mat_gemm(1.0f, &a, &b, 0.0f, &c);
    }
    t1 = now_s();
    flops = 2.0 * n * n * n * reps;
    printf("gemm  %4d  %8.3f ms  %8.2f GFLOP/s\n", n, (t1 - t0) * 1e3, flops / (t1 - t0) / 1e9);
    mat_free(&a);
    mat_free(&b);
    mat_free(&c);
}

static void bench_vec3(int reps) {
    vec3f a = vec3f_new(1, 2, 3);
    vec3f b = vec3f_new(4, 5, 6);
    vec3f c;
    int r;
    double t0, t1;
    t0 = now_s();
    for (r = 0; r < reps; ++r) {
        c = vec3f_cross(vec3f_add(a, b), a);
        a = vec3f_normalize(c);
    }
    t1 = now_s();
    printf("vec3  %4d  %8.3f ms  (sink %.4f)\n", reps, (t1 - t0) * 1e3, a.x);
}

int main(void) {
    printf("algo     N     time           rate\n");
    bench_gemm(4, 200000);
    bench_gemm(16, 20000);
    bench_gemm(64, 400);
    bench_gemm(128, 80);
    bench_vec3(1000000);
    return 0;
}
