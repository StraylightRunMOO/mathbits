/*
 * mathbits - mat.h
 * Row-major matrix views, gemv/gemm, fixed-size types.
 */

#ifndef MATHBITS_MAT_H
#define MATHBITS_MAT_H

#include "vec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATHBITS_M_R 2
#define MATHBITS_M_C 2
#define MATHBITS_M_T float
#define MATHBITS_M_SFX f
#include "detail/mat_gen.h"

#define MATHBITS_M_R 3
#define MATHBITS_M_C 3
#define MATHBITS_M_T float
#define MATHBITS_M_SFX f
#include "detail/mat_gen.h"

#define MATHBITS_M_R 4
#define MATHBITS_M_C 4
#define MATHBITS_M_T float
#define MATHBITS_M_SFX f
#include "detail/mat_gen.h"

#define MATHBITS_M_R 2
#define MATHBITS_M_C 2
#define MATHBITS_M_T double
#define MATHBITS_M_SFX d
#include "detail/mat_gen.h"

#define MATHBITS_M_R 3
#define MATHBITS_M_C 3
#define MATHBITS_M_T double
#define MATHBITS_M_SFX d
#include "detail/mat_gen.h"

#define MATHBITS_M_R 4
#define MATHBITS_M_C 4
#define MATHBITS_M_T double
#define MATHBITS_M_SFX d
#include "detail/mat_gen.h"

MATHBITS_INLINE mat3f mat3f_scale(vec3f s) { return mat3f_scale_vec(s); }
MATHBITS_INLINE mat3d mat3d_scale(vec3d s) { return mat3d_scale_vec(s); }

typedef struct {
    float *data;
    size_t rows, cols, ld;
} mat_f;

typedef struct {
    double *data;
    size_t rows, cols, ld;
} mat_d;

MATHBITS_INLINE mat_f mat_view(float *data, size_t rows, size_t cols, size_t ld) {
    mat_f a;
    a.data = data;
    a.rows = rows;
    a.cols = cols;
    a.ld = ld;
    return a;
}

MATHBITS_INLINE mat_d mat_view_d(double *data, size_t rows, size_t cols, size_t ld) {
    mat_d a;
    a.data = data;
    a.rows = rows;
    a.cols = cols;
    a.ld = ld;
    return a;
}

MATHBITS_INLINE float *mat_ptr(mat_f *a, size_t r, size_t c) {
    return a->data + r * a->ld + c;
}

MATHBITS_INLINE const float *mat_cptr(const mat_f *a, size_t r, size_t c) {
    return a->data + r * a->ld + c;
}

MATHBITS_INLINE float mat_get(const mat_f *a, size_t r, size_t c) {
    return *mat_cptr(a, r, c);
}

MATHBITS_INLINE void mat_set(mat_f *a, size_t r, size_t c, float v) {
    *mat_ptr(a, r, c) = v;
}

MATHBITS_INLINE mat_f mat_alloc(size_t rows, size_t cols) {
    mat_f a;
    a.data = (float *)mathbits_malloc(rows * cols * sizeof(float));
    a.rows = a.data ? rows : 0;
    a.cols = a.data ? cols : 0;
    a.ld = cols;
    return a;
}

MATHBITS_INLINE void mat_free(mat_f *a) {
    if (!a || !a->data) {
        return;
    }
    mathbits_free(a->data, a->rows * a->ld * sizeof(float));
    a->data = NULL;
    a->rows = a->cols = a->ld = 0;
}

MATHBITS_INLINE void mat_fill(mat_f *a, float s) {
    size_t i, j;
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            mat_set(a, i, j, s);
        }
    }
}

MATHBITS_INLINE void mat_set_identity(mat_f *a) {
    size_t i, j, n = a->rows < a->cols ? a->rows : a->cols;
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            mat_set(a, i, j, 0.0f);
        }
    }
    for (i = 0; i < n; ++i) {
        mat_set(a, i, i, 1.0f);
    }
}

MATHBITS_INLINE vec_f mat_row(mat_f *a, size_t r) {
    return vec_view(a->data + r * a->ld, a->cols);
}

MATHBITS_INLINE vec_f mat_col_copy_view(mat_f *a, size_t c) {
    /* strided column is not a contiguous vec_f; callers that need a
     * contiguous column should copy. This returns a 1-strided view only
     * when ld == 1 (rare). For general ld, we expose mat_col_get. */
    (void)c;
    vec_f v;
    v.data = a->data;
    v.n = a->rows;
    return v;
}

MATHBITS_INLINE float mat_col_get(const mat_f *a, size_t r, size_t c) {
    return mat_get(a, r, c);
}

MATHBITS_INLINE mat_f mat_block(mat_f *a, size_t r0, size_t c0, size_t rows, size_t cols) {
    return mat_view(a->data + r0 * a->ld + c0, rows, cols, a->ld);
}

MATHBITS_INLINE vec_f mat_diagonal(mat_f *a) {
    /* Diagonal is strided by ld+1. We do not pretend it is contiguous.
     * This helper copies nothing; use mat_trace for the reduction. */
    return vec_view(a->data, a->rows < a->cols ? a->rows : a->cols);
}

MATHBITS_INLINE mat_f mat_transpose_view(mat_f *a) {
    /* True transpose view needs column-major interpretation. We encode
     * it as rows/cols swapped with ld kept; gemm/gemv treat row-major
     * only. For v0.1, this returns a swapped shape over the same buffer
     * which is correct only for element-wise ops. Prefer mat_transpose. */
    return mat_view(a->data, a->cols, a->rows, a->ld);
}

MATHBITS_INLINE void mat_transpose(const mat_f *a, mat_f *at) {
    size_t i, j;
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            mat_set(at, j, i, mat_get(a, i, j));
        }
    }
}

MATHBITS_INLINE void mat_add_inplace(mat_f *a, const mat_f *b) {
    size_t i, j;
    size_t rows = a->rows < b->rows ? a->rows : b->rows;
    size_t cols = a->cols < b->cols ? a->cols : b->cols;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            mat_set(a, i, j, mat_get(a, i, j) + mat_get(b, i, j));
        }
    }
}

MATHBITS_INLINE void mat_scale_inplace(mat_f *a, float s) {
    size_t i, j;
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            mat_set(a, i, j, mat_get(a, i, j) * s);
        }
    }
}

MATHBITS_INLINE void mat_mul_elementwise(const mat_f *a, const mat_f *b, mat_f *c) {
    size_t i, j;
    size_t rows = a->rows < b->rows ? a->rows : b->rows;
    size_t cols = a->cols < b->cols ? a->cols : b->cols;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            mat_set(c, i, j, mat_get(a, i, j) * mat_get(b, i, j));
        }
    }
}

MATHBITS_INLINE float mat_trace(const mat_f *a) {
    size_t i, n = a->rows < a->cols ? a->rows : a->cols;
    float t = 0.0f;
    for (i = 0; i < n; ++i) {
        t += mat_get(a, i, i);
    }
    return t;
}

MATHBITS_INLINE float mat_norm_frobenius(const mat_f *a) {
    size_t i, j;
    float s = 0.0f;
    for (i = 0; i < a->rows; ++i) {
        for (j = 0; j < a->cols; ++j) {
            float v = mat_get(a, i, j);
            s += v * v;
        }
    }
    return sqrtf(s);
}

MATHBITS_INLINE float mat_norm_inf(const mat_f *a) {
    size_t i, j;
    float best = 0.0f;
    for (i = 0; i < a->rows; ++i) {
        float row = 0.0f;
        for (j = 0; j < a->cols; ++j) {
            row += fabsf(mat_get(a, i, j));
        }
        if (row > best) {
            best = row;
        }
    }
    return best;
}

MATHBITS_INLINE float mat_norm_1(const mat_f *a) {
    size_t i, j;
    float best = 0.0f;
    for (j = 0; j < a->cols; ++j) {
        float col = 0.0f;
        for (i = 0; i < a->rows; ++i) {
            col += fabsf(mat_get(a, i, j));
        }
        if (col > best) {
            best = col;
        }
    }
    return best;
}

MATHBITS_INLINE void mat_center_columns(const mat_f *x, mat_f *xc) {
    size_t i, j;
    for (j = 0; j < x->cols; ++j) {
        float mean = 0.0f;
        for (i = 0; i < x->rows; ++i) {
            mean += mat_get(x, i, j);
        }
        mean /= (float)(x->rows ? x->rows : 1);
        for (i = 0; i < x->rows; ++i) {
            mat_set(xc, i, j, mat_get(x, i, j) - mean);
        }
    }
}

/* y = alpha * A * x + beta * y */
MATHBITS_INLINE void mat_gemv(float alpha, const mat_f *a, const vec_f *x, float beta, vec_f *y) {
    size_t i, k;
    size_t m = a->rows;
    size_t n = a->cols < x->n ? a->cols : x->n;
    for (i = 0; i < m && i < y->n; ++i) {
        const float *ai = a->data + i * a->ld;
        float acc = 0.0f;
        k = 0;
#if MATHBITS_HAS_AVX2
        {
            __m256 vacc = _mm256_setzero_ps();
            for (; k + 8 <= n; k += 8) {
                __m256 va = _mm256_loadu_ps(ai + k);
                __m256 vx = _mm256_loadu_ps(x->data + k);
                vacc = _mm256_fmadd_ps(va, vx, vacc);
            }
            float tmp[8];
            _mm256_storeu_ps(tmp, vacc);
            acc = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
        }
#elif MATHBITS_HAS_NEON
        {
            float32x4_t vacc = vdupq_n_f32(0.0f);
            for (; k + 4 <= n; k += 4) {
                vacc = vmlaq_f32(vacc, vld1q_f32(ai + k), vld1q_f32(x->data + k));
            }
            acc = vgetq_lane_f32(vacc, 0) + vgetq_lane_f32(vacc, 1)
                + vgetq_lane_f32(vacc, 2) + vgetq_lane_f32(vacc, 3);
        }
#endif
        for (; k < n; ++k) {
            acc += ai[k] * x->data[k];
        }
        y->data[i] = alpha * acc + beta * y->data[i];
    }
}

MATHBITS_INLINE float mat_gemv_residual_norm(const mat_f *a, const vec_f *x, const vec_f *b) {
    size_t i;
    float s = 0.0f;
    MATHBITS_FVEC_STACK(256, y);
    vec_f yy = y;
    if (a->rows > 256) {
        yy = vec_alloc(a->rows);
    } else {
        yy.n = a->rows;
    }
    vec_fill(&yy, 0.0f);
    mat_gemv(1.0f, a, x, 0.0f, &yy);
    for (i = 0; i < a->rows && i < b->n; ++i) {
        float r = yy.data[i] - b->data[i];
        s += r * r;
    }
    if (a->rows > 256) {
        vec_free(&yy);
    }
    return sqrtf(s);
}

/* C = alpha * A * B + beta * C   A: m×k, B: k×n, C: m×n, row-major */
MATHBITS_INLINE void mat_gemm(float alpha, const mat_f *a, const mat_f *b, float beta, mat_f *c) {
    size_t i, j, k;
    size_t m = a->rows;
    size_t kk = a->cols < b->rows ? a->cols : b->rows;
    size_t n = b->cols;
    if (c->rows < m) {
        m = c->rows;
    }
    if (c->cols < n) {
        n = c->cols;
    }

    if (beta == 0.0f) {
        for (i = 0; i < m; ++i) {
            float *ci = c->data + i * c->ld;
            for (j = 0; j < n; ++j) {
                ci[j] = 0.0f;
            }
        }
    } else if (beta != 1.0f) {
        for (i = 0; i < m; ++i) {
            float *ci = c->data + i * c->ld;
            for (j = 0; j < n; ++j) {
                ci[j] *= beta;
            }
        }
    }

    for (i = 0; i < m; ++i) {
        const float *ai = a->data + i * a->ld;
        float *ci = c->data + i * c->ld;
        for (k = 0; k < kk; ++k) {
            float aik = ai[k] * alpha;
            const float *bk = b->data + k * b->ld;
            j = 0;
#if MATHBITS_HAS_AVX2
            {
                __m256 va = _mm256_set1_ps(aik);
                for (; j + 8 <= n; j += 8) {
                    __m256 vc = _mm256_loadu_ps(ci + j);
                    __m256 vb = _mm256_loadu_ps(bk + j);
                    _mm256_storeu_ps(ci + j, _mm256_fmadd_ps(va, vb, vc));
                }
            }
#elif MATHBITS_HAS_NEON
            {
                float32x4_t va = vdupq_n_f32(aik);
                for (; j + 4 <= n; j += 4) {
                    float32x4_t vc = vld1q_f32(ci + j);
                    float32x4_t vb = vld1q_f32(bk + j);
                    vst1q_f32(ci + j, vmlaq_f32(vc, va, vb));
                }
            }
#endif
            for (; j < n; ++j) {
                ci[j] += aik * bk[j];
            }
        }
    }
}

#ifdef MATHBITS_USE_CBLAS
/* Optional trampoline — link a CBLAS when MATHBITS_USE_CBLAS is defined. */
void cblas_sgemm(int, int, int, int, int, int, float, const float *, int,
                 const float *, int, float, float *, int);
#endif

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_MAT_H */
