#ifndef MATHBITS_MAT_TRANSITION_H
#define MATHBITS_MAT_TRANSITION_H

#include "adjacency.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef mat_f transition_f;

MATHBITS_INLINE transition_f transition_row_stochastic(const adjacency_f *a) {
    mat_f d = sparse_to_dense(a);
    size_t i, j;
    for (i = 0; i < d.rows; ++i) {
        float s = 0.0f;
        for (j = 0; j < d.cols; ++j) {
            s += mat_get(&d, i, j);
        }
        if (s == 0.0f) {
            continue;
        }
        for (j = 0; j < d.cols; ++j) {
            mat_set(&d, i, j, mat_get(&d, i, j) / s);
        }
    }
    return d;
}

MATHBITS_INLINE int transition_stationary(const transition_f *p, vec_f *pi, int iters, float tol) {
    vec_f x = vec_alloc(p->rows);
    vec_f y = vec_alloc(p->rows);
    int t, i;
    if (!x.data || !y.data) {
        vec_free(&x);
        vec_free(&y);
        return 0;
    }
    vec_fill(&x, 1.0f / (float)p->rows);
    for (t = 0; t < iters; ++t) {
        vec_fill(&y, 0.0f);
        /* pi_{k+1} = pi_k P  (row vector) => y_j = sum_i x_i P_ij */
        for (i = 0; (size_t)i < p->rows; ++i) {
            size_t j;
            for (j = 0; j < p->cols; ++j) {
                y.data[j] += x.data[i] * mat_get(p, (size_t)i, j);
            }
        }
        {
            float nrm = 0.0f;
            for (i = 0; (size_t)i < x.n; ++i) {
                float d = y.data[i] - x.data[i];
                nrm += d * d;
            }
            memcpy(x.data, y.data, x.n * sizeof(float));
            if (sqrtf(nrm) < tol) {
                break;
            }
        }
    }
    memcpy(pi->data, x.data, (x.n < pi->n ? x.n : pi->n) * sizeof(float));
    vec_free(&x);
    vec_free(&y);
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_MAT_TRANSITION_H */
