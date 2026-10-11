/*
 * mathbits - linalg.h
 * Small/medium dense factorizations. Caller supplies outputs and scratch.
 */

#ifndef MATHBITS_LINALG_H
#define MATHBITS_LINALG_H

#include "mat.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A = L L^T, L lower triangular written to L (may alias A). Returns 0 if not SPD. */
MATHBITS_INLINE int linalg_cholesky(const mat_f *a, mat_f *l) {
    size_t n = a->rows < a->cols ? a->rows : a->cols;
    size_t i, j, k;
    if (l->rows < n || l->cols < n) {
        return 0;
    }
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(l, i, j, 0.0f);
        }
    }
    for (i = 0; i < n; ++i) {
        for (j = 0; j <= i; ++j) {
            float sum = mat_get(a, i, j);
            for (k = 0; k < j; ++k) {
                sum -= mat_get(l, i, k) * mat_get(l, j, k);
            }
            if (i == j) {
                if (sum <= 0.0f) {
                    return 0;
                }
                mat_set(l, i, j, sqrtf(sum));
            } else {
                float d = mat_get(l, j, j);
                if (d == 0.0f) {
                    return 0;
                }
                mat_set(l, i, j, sum / d);
            }
        }
    }
    return 1;
}

MATHBITS_INLINE void linalg_solve_triangular(const mat_f *l, const vec_f *b, vec_f *x, int lower) {
    size_t n = l->rows < b->n ? l->rows : b->n;
    size_t i, j;
    if (lower) {
        for (i = 0; i < n; ++i) {
            float s = b->data[i];
            for (j = 0; j < i; ++j) {
                s -= mat_get(l, i, j) * x->data[j];
            }
            x->data[i] = s / mat_get(l, i, i);
        }
    } else {
        size_t ii;
        for (ii = n; ii-- > 0;) {
            float s = b->data[ii];
            for (j = ii + 1; j < n; ++j) {
                s -= mat_get(l, ii, j) * x->data[j];
            }
            x->data[ii] = s / mat_get(l, ii, ii);
        }
    }
}

/* Householder QR: A (m×n, m>=n) -> Q (m×n thin) and R (n×n). Scratch: m floats. */
MATHBITS_INLINE int linalg_qr(const mat_f *a, mat_f *q, mat_f *r, vec_f *work) {
    size_t m = a->rows;
    size_t n = a->cols;
    size_t i, j, k;
    if (!q || !r || q->rows < m || q->cols < n || r->rows < n || r->cols < n) {
        return 0;
    }
    /* Copy A into Q; R starts zero. */
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(q, i, j, mat_get(a, i, j));
        }
    }
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(r, i, j, 0.0f);
        }
    }
    for (k = 0; k < n; ++k) {
        float norm = 0.0f;
        float s, inv;
        for (i = k; i < m; ++i) {
            float v = mat_get(q, i, k);
            norm += v * v;
        }
        norm = sqrtf(norm);
        if (norm == 0.0f) {
            return 0;
        }
        if (mat_get(q, k, k) < 0.0f) {
            norm = -norm;
        }
        mat_set(r, k, k, -norm);
        inv = 1.0f / (mat_get(q, k, k) - (-norm)); /* Householder scale via modified col */
        /* Build v in column k of Q: v = x + sign(x1)*||x|| e1, then normalize later. */
        {
            float xk = mat_get(q, k, k);
            float u1 = xk + (xk >= 0.0f ? norm : -norm);
            if (u1 == 0.0f) {
                continue;
            }
            mat_set(q, k, k, u1);
            s = 0.0f;
            for (i = k; i < m; ++i) {
                float v = mat_get(q, i, k);
                s += v * v;
            }
            s = sqrtf(s);
            if (s == 0.0f) {
                continue;
            }
            for (i = k; i < m; ++i) {
                mat_set(q, i, k, mat_get(q, i, k) / s);
            }
        }
        (void)inv;
        (void)work;
        for (j = k + 1; j < n; ++j) {
            float dot = 0.0f;
            for (i = k; i < m; ++i) {
                dot += mat_get(q, i, k) * mat_get(q, i, j);
            }
            dot *= 2.0f;
            for (i = k; i < m; ++i) {
                mat_set(q, i, j, mat_get(q, i, j) - dot * mat_get(q, i, k));
            }
            mat_set(r, k, j, mat_get(q, k, j)); /* not quite — fill R below */
        }
    }
    /* Better: classical Gram-Schmidt (numerically OK for small n). */
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(q, i, j, mat_get(a, i, j));
        }
    }
    for (j = 0; j < n; ++j) {
        for (k = 0; k < j; ++k) {
            float dot = 0.0f;
            for (i = 0; i < m; ++i) {
                dot += mat_get(q, i, k) * mat_get(a, i, j);
            }
            mat_set(r, k, j, dot);
            for (i = 0; i < m; ++i) {
                mat_set(q, i, j, mat_get(q, i, j) - dot * mat_get(q, i, k));
            }
        }
        {
            float nrm = 0.0f;
            for (i = 0; i < m; ++i) {
                float v = mat_get(q, i, j);
                nrm += v * v;
            }
            nrm = sqrtf(nrm);
            if (nrm < 1e-20f) {
                return 0;
            }
            mat_set(r, j, j, nrm);
            for (i = 0; i < m; ++i) {
                mat_set(q, i, j, mat_get(q, i, j) / nrm);
            }
        }
        for (k = j + 1; k < n; ++k) {
            /* R upper already filled for k<j; remaining zeros */
        }
    }
    return 1;
}

MATHBITS_INLINE int linalg_solve(const mat_f *a, const vec_f *b, vec_f *x) {
    size_t n = a->rows;
    size_t i, j, p, k;
    mat_f m;
    vec_f y;
    float *buf;
    if (a->rows != a->cols || b->n < n || x->n < n) {
        return 0;
    }
    buf = (float *)mathbits_malloc(n * n * sizeof(float) + n * sizeof(float));
    if (!buf) {
        return 0;
    }
    m = mat_view(buf, n, n, n);
    y = vec_view(buf + n * n, n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(&m, i, j, mat_get(a, i, j));
        }
        y.data[i] = b->data[i];
    }
    /* Partial-pivot LU in place, then solve. */
    for (k = 0; k < n; ++k) {
        float best = fabsf(mat_get(&m, k, k));
        p = k;
        for (i = k + 1; i < n; ++i) {
            float v = fabsf(mat_get(&m, i, k));
            if (v > best) {
                best = v;
                p = i;
            }
        }
        if (best == 0.0f) {
            mathbits_free(buf, n * n * sizeof(float) + n * sizeof(float));
            return 0;
        }
        if (p != k) {
            for (j = 0; j < n; ++j) {
                float t = mat_get(&m, k, j);
                mat_set(&m, k, j, mat_get(&m, p, j));
                mat_set(&m, p, j, t);
            }
            {
                float t = y.data[k];
                y.data[k] = y.data[p];
                y.data[p] = t;
            }
        }
        for (i = k + 1; i < n; ++i) {
            float f = mat_get(&m, i, k) / mat_get(&m, k, k);
            mat_set(&m, i, k, f);
            for (j = k + 1; j < n; ++j) {
                mat_set(&m, i, j, mat_get(&m, i, j) - f * mat_get(&m, k, j));
            }
            y.data[i] -= f * y.data[k];
        }
    }
    for (i = n; i-- > 0;) {
        float s = y.data[i];
        for (j = i + 1; j < n; ++j) {
            s -= mat_get(&m, i, j) * x->data[j];
        }
        x->data[i] = s / mat_get(&m, i, i);
    }
    mathbits_free(buf, n * n * sizeof(float) + n * sizeof(float));
    return 1;
}

MATHBITS_INLINE int linalg_lstsq(const mat_f *x, const vec_f *y, vec_f *beta) {
    /* Normal equations: (X^T X) beta = X^T y  for small n. */
    size_t m = x->rows;
    size_t n = x->cols;
    size_t i, j, k;
    mat_f xtx;
    vec_f xty;
    float *buf;
    int ok;
    if (beta->n < n) {
        return 0;
    }
    buf = (float *)mathbits_malloc((n * n + n) * sizeof(float));
    if (!buf) {
        return 0;
    }
    xtx = mat_view(buf, n, n, n);
    xty = vec_view(buf + n * n, n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            float s = 0.0f;
            for (k = 0; k < m; ++k) {
                s += mat_get(x, k, i) * mat_get(x, k, j);
            }
            mat_set(&xtx, i, j, s);
        }
        {
            float s = 0.0f;
            for (k = 0; k < m; ++k) {
                s += mat_get(x, k, i) * y->data[k];
            }
            xty.data[i] = s;
        }
    }
    ok = linalg_solve(&xtx, &xty, beta);
    mathbits_free(buf, (n * n + n) * sizeof(float));
    return ok;
}

MATHBITS_INLINE int linalg_ols(const mat_f *x, const vec_f *y, vec_f *beta) {
    return linalg_lstsq(x, y, beta);
}

MATHBITS_INLINE int linalg_ridge(const mat_f *x, const vec_f *y, float lambda, vec_f *beta) {
    size_t m = x->rows;
    size_t n = x->cols;
    size_t i, j, k;
    mat_f xtx;
    vec_f xty;
    float *buf;
    int ok;
    buf = (float *)mathbits_malloc((n * n + n) * sizeof(float));
    if (!buf) {
        return 0;
    }
    xtx = mat_view(buf, n, n, n);
    xty = vec_view(buf + n * n, n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            float s = 0.0f;
            for (k = 0; k < m; ++k) {
                s += mat_get(x, k, i) * mat_get(x, k, j);
            }
            if (i == j) {
                s += lambda;
            }
            mat_set(&xtx, i, j, s);
        }
        {
            float s = 0.0f;
            for (k = 0; k < m; ++k) {
                s += mat_get(x, k, i) * y->data[k];
            }
            xty.data[i] = s;
        }
    }
    ok = linalg_solve(&xtx, &xty, beta);
    mathbits_free(buf, (n * n + n) * sizeof(float));
    return ok;
}

/* Jacobi eigendecomposition of a symmetric n×n matrix. evals, evecs (n×n). */
MATHBITS_INLINE int linalg_syev(const mat_f *a, vec_f *evals, mat_f *evecs) {
    size_t n = a->rows;
    size_t i, j, p, q, iter;
    mat_f m;
    float *buf;
    if (a->rows != a->cols || evals->n < n || evecs->rows < n || evecs->cols < n) {
        return 0;
    }
    buf = (float *)mathbits_malloc(n * n * sizeof(float));
    if (!buf) {
        return 0;
    }
    m = mat_view(buf, n, n, n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            mat_set(&m, i, j, mat_get(a, i, j));
        }
    }
    mat_set_identity(evecs);
    for (iter = 0; iter < n * n * 8; ++iter) {
        float maxv = 0.0f;
        p = 0;
        q = 1;
        for (i = 0; i < n; ++i) {
            for (j = i + 1; j < n; ++j) {
                float v = fabsf(mat_get(&m, i, j));
                if (v > maxv) {
                    maxv = v;
                    p = i;
                    q = j;
                }
            }
        }
        if (maxv < 1e-8f) {
            break;
        }
        {
            float app = mat_get(&m, p, p);
            float aqq = mat_get(&m, q, q);
            float apq = mat_get(&m, p, q);
            float tau = (aqq - app) / (2.0f * apq);
            float t = (tau >= 0.0f ? 1.0f : -1.0f) / (fabsf(tau) + sqrtf(1.0f + tau * tau));
            float c = 1.0f / sqrtf(1.0f + t * t);
            float s = t * c;
            for (i = 0; i < n; ++i) {
                if (i != p && i != q) {
                    float aip = mat_get(&m, i, p);
                    float aiq = mat_get(&m, i, q);
                    mat_set(&m, i, p, c * aip - s * aiq);
                    mat_set(&m, p, i, c * aip - s * aiq);
                    mat_set(&m, i, q, s * aip + c * aiq);
                    mat_set(&m, q, i, s * aip + c * aiq);
                }
            }
            mat_set(&m, p, p, app - t * apq);
            mat_set(&m, q, q, aqq + t * apq);
            mat_set(&m, p, q, 0.0f);
            mat_set(&m, q, p, 0.0f);
            for (i = 0; i < n; ++i) {
                float vip = mat_get(evecs, i, p);
                float viq = mat_get(evecs, i, q);
                mat_set(evecs, i, p, c * vip - s * viq);
                mat_set(evecs, i, q, s * vip + c * viq);
            }
        }
    }
    for (i = 0; i < n; ++i) {
        evals->data[i] = mat_get(&m, i, i);
    }
    mathbits_free(buf, n * n * sizeof(float));
    return 1;
}

/* Thin SVD via A^T A eig: A (m×n, m>=n) -> U (m×n), S (n), Vt (n×n). */
MATHBITS_INLINE int linalg_svd(const mat_f *a, mat_f *u, vec_f *s, mat_f *vt) {
    size_t m = a->rows;
    size_t n = a->cols;
    size_t i, j, k;
    mat_f ata, evecs;
    vec_f evals;
    float *buf;
    int ok;
    if (m < n) {
        return 0;
    }
    buf = (float *)mathbits_malloc((n * n * 2 + n) * sizeof(float));
    if (!buf) {
        return 0;
    }
    ata = mat_view(buf, n, n, n);
    evecs = mat_view(buf + n * n, n, n, n);
    evals = vec_view(buf + 2 * n * n, n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            float acc = 0.0f;
            for (k = 0; k < m; ++k) {
                acc += mat_get(a, k, i) * mat_get(a, k, j);
            }
            mat_set(&ata, i, j, acc);
        }
    }
    ok = linalg_syev(&ata, &evals, &evecs);
    if (!ok) {
        mathbits_free(buf, (n * n * 2 + n) * sizeof(float));
        return 0;
    }
    /* Sort descending. */
    for (i = 0; i < n; ++i) {
        size_t best = i;
        for (j = i + 1; j < n; ++j) {
            if (evals.data[j] > evals.data[best]) {
                best = j;
            }
        }
        if (best != i) {
            float t = evals.data[i];
            evals.data[i] = evals.data[best];
            evals.data[best] = t;
            for (k = 0; k < n; ++k) {
                float uij = mat_get(&evecs, k, i);
                mat_set(&evecs, k, i, mat_get(&evecs, k, best));
                mat_set(&evecs, k, best, uij);
            }
        }
    }
    for (i = 0; i < n; ++i) {
        float ev = evals.data[i] < 0.0f ? 0.0f : evals.data[i];
        s->data[i] = sqrtf(ev);
    }
    if (vt) {
        for (i = 0; i < n; ++i) {
            for (j = 0; j < n; ++j) {
                mat_set(vt, i, j, mat_get(&evecs, j, i)); /* V^T */
            }
        }
    }
    if (u) {
        for (j = 0; j < n; ++j) {
            float sj = s->data[j];
            float inv = sj > 1e-12f ? 1.0f / sj : 0.0f;
            for (i = 0; i < m; ++i) {
                float acc = 0.0f;
                for (k = 0; k < n; ++k) {
                    acc += mat_get(a, i, k) * mat_get(&evecs, k, j);
                }
                mat_set(u, i, j, acc * inv);
            }
        }
    }
    mathbits_free(buf, (n * n * 2 + n) * sizeof(float));
    return 1;
}

MATHBITS_INLINE int linalg_covariance(const mat_f *xc, mat_f *cov) {
    size_t n = xc->rows;
    size_t p = xc->cols;
    size_t i, j, k;
    float denom = n > 1 ? (float)(n - 1) : 1.0f;
    for (i = 0; i < p; ++i) {
        for (j = 0; j < p; ++j) {
            float s = 0.0f;
            for (k = 0; k < n; ++k) {
                s += mat_get(xc, k, i) * mat_get(xc, k, j);
            }
            mat_set(cov, i, j, s / denom);
        }
    }
    return 1;
}

MATHBITS_INLINE int linalg_pca(const mat_f *xc, size_t k, mat_f *components, vec_f *explained) {
    size_t p = xc->cols;
    mat_f u, vt, cov;
    vec_f s;
    float *buf;
    size_t i, j;
    int ok;
    buf = (float *)mathbits_malloc((p * p * 3 + p) * sizeof(float));
    if (!buf) {
        return 0;
    }
    cov = mat_view(buf, p, p, p);
    vt = mat_view(buf + p * p, p, p, p);
    u = mat_view(buf + 2 * p * p, p, p, p);
    s = vec_view(buf + 3 * p * p, p);
    linalg_covariance(xc, &cov);
    ok = linalg_svd(&cov, &u, &s, &vt);
    if (ok) {
        if (k > p) {
            k = p;
        }
        for (i = 0; i < p; ++i) {
            for (j = 0; j < k; ++j) {
                mat_set(components, i, j, mat_get(&u, i, j));
            }
        }
        if (explained) {
            for (j = 0; j < k && j < explained->n; ++j) {
                explained->data[j] = s.data[j];
            }
        }
    }
    mathbits_free(buf, (p * p * 3 + p) * sizeof(float));
    return ok;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_LINALG_H */
