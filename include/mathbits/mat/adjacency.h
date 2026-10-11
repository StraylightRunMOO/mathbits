#ifndef MATHBITS_MAT_ADJACENCY_H
#define MATHBITS_MAT_ADJACENCY_H

#include "sparse.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef sparse_mat_f adjacency_f;

MATHBITS_INLINE adjacency_f adjacency_from_edges(const uint32_t edges[][2], size_t nedges, size_t nnodes) {
    uint32_t *rows = (uint32_t *)mathbits_malloc(nedges * 2 * sizeof(uint32_t));
    uint32_t *cols = (uint32_t *)mathbits_malloc(nedges * 2 * sizeof(uint32_t));
    float *vals = (float *)mathbits_malloc(nedges * 2 * sizeof(float));
    size_t i, nnz = 0;
    adjacency_f a;
    for (i = 0; i < nedges; ++i) {
        rows[nnz] = edges[i][0];
        cols[nnz] = edges[i][1];
        vals[nnz] = 1.0f;
        nnz++;
        if (edges[i][0] != edges[i][1]) {
            rows[nnz] = edges[i][1];
            cols[nnz] = edges[i][0];
            vals[nnz] = 1.0f;
            nnz++;
        }
    }
    a = sparse_from_triplets(rows, cols, vals, nnz, nnodes, nnodes);
    mathbits_free(rows, nedges * 2 * sizeof(uint32_t));
    mathbits_free(cols, nedges * 2 * sizeof(uint32_t));
    mathbits_free(vals, nedges * 2 * sizeof(float));
    return a;
}

MATHBITS_INLINE mat_f adjacency_to_dense(const adjacency_f *a) {
    return sparse_to_dense(a);
}

MATHBITS_INLINE sparse_mat_f adjacency_to_sparse(const adjacency_f *a) {
    return *a;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_MAT_ADJACENCY_H */
