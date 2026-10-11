#include "mathbits/mathbits.h"
#include "test_common.h"

int test_linalg(void) {
    float astor[16], lstor[16], qstor[16], rstor[16], xstor[4], ystor[4], bstor[4];
    mat_f a = mat_view(astor, 3, 3, 3);
    mat_f l = mat_view(lstor, 3, 3, 3);
    mat_f q = mat_view(qstor, 3, 3, 3);
    mat_f r = mat_view(rstor, 3, 3, 3);
    vec_f x = vec_view(xstor, 3);
    vec_f y = vec_view(ystor, 3);
    vec_f b = vec_view(bstor, 3);
    int i, j, k;
    float rec;

    /* SPD: A = M^T M with M well conditioned */
    mat_set_identity(&a);
    mat_set(&a, 0, 0, 4);
    mat_set(&a, 1, 1, 5);
    mat_set(&a, 2, 2, 6);
    mat_set(&a, 0, 1, 1);
    mat_set(&a, 1, 0, 1);
    mat_set(&a, 1, 2, 0.5f);
    mat_set(&a, 2, 1, 0.5f);
    ASSERT(linalg_cholesky(&a, &l));
    rec = 0.0f;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            float s = 0.0f;
            for (k = 0; k < 3; ++k) {
                s += mat_get(&l, i, k) * mat_get(&l, j, k);
            }
            rec += fabsf(s - mat_get(&a, i, j));
        }
    }
    ASSERT(rec < 1e-4f);

    b.data[0] = 1;
    b.data[1] = 2;
    b.data[2] = 3;
    ASSERT(linalg_solve(&a, &b, &x));
    vec_fill(&y, 0);
    mat_gemv(1.0f, &a, &x, 0.0f, &y);
    ASSERT_NEAR(y.data[0], 1.0f, 1e-4);
    ASSERT_NEAR(y.data[1], 2.0f, 1e-4);
    ASSERT_NEAR(y.data[2], 3.0f, 1e-4);

    ASSERT(linalg_qr(&a, &q, &r, NULL));
    /* Q^T Q ~ I */
    {
        float qtq00 = 0;
        for (i = 0; i < 3; ++i) {
            qtq00 += mat_get(&q, i, 0) * mat_get(&q, i, 0);
        }
        ASSERT_NEAR(qtq00, 1.0f, 1e-4);
    }

    {
        float Xb[20], yb[4], betab[2];
        mat_f X = mat_view(Xb, 4, 2, 2);
        vec_f yy = vec_view(yb, 4);
        vec_f beta = vec_view(betab, 2);
        for (i = 0; i < 4; ++i) {
            mat_set(&X, (size_t)i, 0, 1.0f);
            mat_set(&X, (size_t)i, 1, (float)i);
            yy.data[i] = 2.0f + 3.0f * (float)i;
        }
        ASSERT(linalg_lstsq(&X, &yy, &beta));
        ASSERT_NEAR(beta.data[0], 2.0f, 1e-3);
        ASSERT_NEAR(beta.data[1], 3.0f, 1e-3);
    }

    {
        mat3f A3 = mat3f_new(2, 0, 0, 0, 3, 0, 0, 0, 4);
        mat3f inv;
        ASSERT(mat3f_inverse(&A3, &inv));
        ASSERT_NEAR(inv.m[0][0], 0.5f, 1e-5);
        ASSERT_NEAR(inv.m[1][1], 1.0f / 3.0f, 1e-5);
    }

    printf("PASS test_linalg\n");
    return 0;
}
