#include "mathbits/mathbits.h"
#include "test_common.h"

static void naive_gemm(const mat_f *a, const mat_f *b, mat_f *c) {
    size_t i, j, k;
    mat_fill(c, 0.0f);
    for (i = 0; i < a->rows; ++i) {
        for (k = 0; k < a->cols; ++k) {
            for (j = 0; j < b->cols; ++j) {
                mat_set(c, i, j, mat_get(c, i, j) + mat_get(a, i, k) * mat_get(b, k, j));
            }
        }
    }
}

int test_mat(void) {
    float astor[16], bstor[16], cstor[16], nstor[16], xstor[4], ystor[4];
    mat_f a = mat_view(astor, 3, 3, 3);
    mat_f b = mat_view(bstor, 3, 3, 3);
    mat_f c = mat_view(cstor, 3, 3, 3);
    mat_f n = mat_view(nstor, 3, 3, 3);
    vec_f x = vec_view(xstor, 3);
    vec_f y = vec_view(ystor, 3);
    size_t i, j;
    mat2f a2, inv2;
    mat3f R, S, RS;
    vec3f v, w;

    mat_set_identity(&a);
    ASSERT_NEAR(mat_get(&a, 0, 0), 1.0f, 1e-7);
    ASSERT_NEAR(mat_trace(&a), 3.0f, 1e-6);

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            mat_set(&a, i, j, (float)(i + 1) * 0.3f + (float)j);
            mat_set(&b, i, j, (float)(j + 1) * 0.2f - (float)i * 0.1f);
        }
    }
    mat_gemm(1.0f, &a, &b, 0.0f, &c);
    naive_gemm(&a, &b, &n);
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            ASSERT_NEAR(mat_get(&c, i, j), mat_get(&n, i, j), 1e-5);
        }
    }

    x.data[0] = 1;
    x.data[1] = 2;
    x.data[2] = 3;
    vec_fill(&y, 0);
    mat_gemv(1.0f, &a, &x, 0.0f, &y);
    ASSERT_NEAR(y.data[0],
                mat_get(&a, 0, 0) * 1 + mat_get(&a, 0, 1) * 2 + mat_get(&a, 0, 2) * 3,
                1e-5);

    a2 = mat2f_new(4, 3, 2, 1);
    ASSERT(mat2f_inverse(&a2, &inv2));
    ASSERT_NEAR(mat2f_determinant(a2), -2.0f, 1e-5);

    R = mat3f_identity();
    S = mat3f_scale(vec3f_new(2, 3, 4));
    RS = mat3f_mul(R, S);
    v = vec3f_new(1, 1, 1);
    w = mat3f_mul_vec(RS, v);
    ASSERT_NEAR(w.x, 2.0f, 1e-5);
    ASSERT_NEAR(w.y, 3.0f, 1e-5);
    ASSERT_NEAR(w.z, 4.0f, 1e-5);

    {
        float wide[20];
        mat_f A = mat_view(wide, 2, 3, 5); /* ld != cols */
        mat_fill(&A, 0);
        mat_set(&A, 0, 0, 1);
        mat_set(&A, 0, 2, 2);
        ASSERT_NEAR(mat_get(&A, 0, 2), 2.0f, 1e-7);
        ASSERT(wide[2] == 2.0f);
    }

    printf("PASS test_mat\n");
    return 0;
}
