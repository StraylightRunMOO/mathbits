#ifndef MATHBITS_TEST_COMMON_H
#define MATHBITS_TEST_COMMON_H

#include <stdio.h>
#include <math.h>
#include <string.h>

static int g_fails;

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        g_fails++; \
        return 1; \
    } \
} while (0)

#define ASSERT_NEAR(a, b, eps) do { \
    double _a = (double)(a), _b = (double)(b), _e = (double)(eps); \
    if (fabs(_a - _b) > _e) { \
        printf("FAIL %s:%d: expected %g got %g (tol %g)\n", \
               __FILE__, __LINE__, _b, _a, _e); \
        g_fails++; \
        return 1; \
    } \
} while (0)

int test_num(void);
int test_vec(void);
int test_mat(void);
int test_linalg(void);
int test_tensor(void);
int test_nn(void);
int test_geometry(void);
int test_optim(void);
int test_sparse(void);

#endif
