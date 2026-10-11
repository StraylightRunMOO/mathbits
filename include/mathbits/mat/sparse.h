#ifndef MATHBITS_MAT_SPARSE_H
#define MATHBITS_MAT_SPARSE_H

#include "../mat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float *vals;
    uint32_t *cols;
    uint32_t *rowptr;
    size_t nrows, ncols, nnz;
    int own;
} sparse_mat_f;

MATHBITS_INLINE sparse_mat_f sparse_csr_view(float *vals, uint32_t *cols, uint32_t *rowptr,
                                             size_t nrows, size_t ncols, size_t nnz) {
    sparse_mat_f s;
    s.vals = vals;
    s.cols = cols;
    s.rowptr = rowptr;
    s.nrows = nrows;
    s.ncols = ncols;
    s.nnz = nnz;
    s.own = 0;
    return s;
}

MATHBITS_INLINE void sparse_free(sparse_mat_f *s) {
    if (!s || !s->own) {
        return;
    }
    mathbits_free(s->vals, s->nnz * sizeof(float));
    mathbits_free(s->cols, s->nnz * sizeof(uint32_t));
    mathbits_free(s->rowptr, (s->nrows + 1) * sizeof(uint32_t));
    s->vals = NULL;
    s->own = 0;
}

MATHBITS_INLINE void sparse_gemv(float alpha, const sparse_mat_f *s, const vec_f *x, float beta, vec_f *y) {
    size_t i, p;
    for (i = 0; i < s->nrows && i < y->n; ++i) {
        float acc = 0.0f;
        for (p = s->rowptr[i]; p < s->rowptr[i + 1]; ++p) {
            acc += s->vals[p] * x->data[s->cols[p]];
        }
        y->data[i] = alpha * acc + beta * y->data[i];
    }
}

MATHBITS_INLINE sparse_mat_f sparse_from_triplets(const uint32_t *rows, const uint32_t *cols,
                                                  const float *vals, size_t nnz,
                                                  size_t nrows, size_t ncols) {
    sparse_mat_f s;
    size_t i, p;
    uint32_t *counts;
    memset(&s, 0, sizeof(s));
    s.nrows = nrows;
    s.ncols = ncols;
    s.nnz = nnz;
    s.own = 1;
    s.vals = (float *)mathbits_malloc(nnz * sizeof(float));
    s.cols = (uint32_t *)mathbits_malloc(nnz * sizeof(uint32_t));
    s.rowptr = (uint32_t *)mathbits_malloc((nrows + 1) * sizeof(uint32_t));
    counts = (uint32_t *)mathbits_malloc(nrows * sizeof(uint32_t));
    if (!s.vals || !s.cols || !s.rowptr || !counts) {
        s.own = 0;
        return s;
    }
    memset(counts, 0, nrows * sizeof(uint32_t));
    for (i = 0; i < nnz; ++i) {
        counts[rows[i]]++;
    }
    s.rowptr[0] = 0;
    for (i = 0; i < nrows; ++i) {
        s.rowptr[i + 1] = s.rowptr[i] + counts[i];
        counts[i] = s.rowptr[i];
    }
    for (i = 0; i < nnz; ++i) {
        p = counts[rows[i]]++;
        s.cols[p] = cols[i];
        s.vals[p] = vals[i];
    }
    mathbits_free(counts, nrows * sizeof(uint32_t));
    return s;
}

MATHBITS_INLINE mat_f sparse_to_dense(const sparse_mat_f *s) {
    mat_f d = mat_alloc(s->nrows, s->ncols);
    size_t i, p;
    if (!d.data) {
        return d;
    }
    mat_fill(&d, 0.0f);
    for (i = 0; i < s->nrows; ++i) {
        for (p = s->rowptr[i]; p < s->rowptr[i + 1]; ++p) {
            mat_set(&d, i, s->cols[p], s->vals[p]);
        }
    }
    return d;
}

MATHBITS_INLINE sparse_mat_f sparse_from_dense(const mat_f *d, float drop) {
    size_t i, j, nnz = 0, p = 0;
    uint32_t *rows;
    uint32_t *cols;
    float *vals;
    sparse_mat_f s;
    for (i = 0; i < d->rows; ++i) {
        for (j = 0; j < d->cols; ++j) {
            if (fabsf(mat_get(d, i, j)) > drop) {
                nnz++;
            }
        }
    }
    rows = (uint32_t *)mathbits_malloc(nnz * sizeof(uint32_t));
    cols = (uint32_t *)mathbits_malloc(nnz * sizeof(uint32_t));
    vals = (float *)mathbits_malloc(nnz * sizeof(float));
    for (i = 0; i < d->rows; ++i) {
        for (j = 0; j < d->cols; ++j) {
            float v = mat_get(d, i, j);
            if (fabsf(v) > drop) {
                rows[p] = (uint32_t)i;
                cols[p] = (uint32_t)j;
                vals[p] = v;
                p++;
            }
        }
    }
    s = sparse_from_triplets(rows, cols, vals, nnz, d->rows, d->cols);
    mathbits_free(rows, nnz * sizeof(uint32_t));
    mathbits_free(cols, nnz * sizeof(uint32_t));
    mathbits_free(vals, nnz * sizeof(float));
    return s;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_MAT_SPARSE_H */
