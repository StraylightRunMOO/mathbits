#ifndef MATHBITS_TENSOR_H
#define MATHBITS_TENSOR_H

#include "mat.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MATHBITS_TENSOR_MAX_RANK
#  define MATHBITS_TENSOR_MAX_RANK 8
#endif

typedef struct {
    float *data;
    size_t shape[MATHBITS_TENSOR_MAX_RANK];
    size_t ndim;
} tensor_f;

MATHBITS_INLINE tensor_f tensor_view(float *data, const size_t *shape, size_t ndim) {
    tensor_f t;
    size_t i;
    t.data = data;
    t.ndim = ndim;
    for (i = 0; i < ndim && i < MATHBITS_TENSOR_MAX_RANK; ++i) {
        t.shape[i] = shape[i];
    }
    return t;
}

MATHBITS_INLINE size_t tensor_numel(const tensor_f *t) {
    size_t n = 1, i;
    for (i = 0; i < t->ndim; ++i) {
        n *= t->shape[i];
    }
    return n;
}

MATHBITS_INLINE size_t tensor_offset(const tensor_f *t, const size_t *idx) {
    size_t off = 0, i, stride = 1;
    for (i = t->ndim; i-- > 0;) {
        off += idx[i] * stride;
        stride *= t->shape[i];
    }
    return off;
}

MATHBITS_INLINE float tensor_get(const tensor_f *t, const size_t *idx) {
    return t->data[tensor_offset(t, idx)];
}

MATHBITS_INLINE void tensor_set(tensor_f *t, const size_t *idx, float v) {
    t->data[tensor_offset(t, idx)] = v;
}

/* Sum along axis 0 of a rank-3 tensor into a matrix (shape[1] × shape[2]). */
MATHBITS_INLINE void tensor_sum_axis(const tensor_f *t, size_t axis, mat_f *out) {
    size_t i, j, k;
    if (t->ndim == 3 && axis == 0) {
        for (j = 0; j < t->shape[1]; ++j) {
            for (k = 0; k < t->shape[2]; ++k) {
                float s = 0.0f;
                for (i = 0; i < t->shape[0]; ++i) {
                    size_t idx[3];
                    idx[0] = i;
                    idx[1] = j;
                    idx[2] = k;
                    s += tensor_get(t, idx);
                }
                mat_set(out, j, k, s);
            }
        }
        return;
    }
    if (t->ndim == 2 && axis == 0) {
        vec_f v = vec_view(out->data, t->shape[1]);
        for (j = 0; j < t->shape[1]; ++j) {
            float s = 0.0f;
            for (i = 0; i < t->shape[0]; ++i) {
                size_t idx[2];
                idx[0] = i;
                idx[1] = j;
                s += tensor_get(t, idx);
            }
            v.data[j] = s;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_TENSOR_H */
