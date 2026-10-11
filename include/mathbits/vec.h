/*
 * mathbits - vec.h
 * Fixed-size vectors (preprocessor-generated) and dynamic views.
 *
 * Include only this header for a complete vector API.
 *
 * Extra widths:
 *   #define MATHBITS_V_N 8
 *   #define MATHBITS_V_T float
 *   #define MATHBITS_V_SFX f
 *   #define MATHBITS_V_T_IS_FLOAT
 *   #include "mathbits/detail/vec_gen.h"
 */

#ifndef MATHBITS_VEC_H
#define MATHBITS_VEC_H

#include "num.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATHBITS_V_N 2
#define MATHBITS_V_T float
#define MATHBITS_V_SFX f
#define MATHBITS_V_T_IS_FLOAT
#include "detail/vec_gen.h"

#define MATHBITS_V_N 3
#define MATHBITS_V_T float
#define MATHBITS_V_SFX f
#define MATHBITS_V_T_IS_FLOAT
#include "detail/vec_gen.h"

#define MATHBITS_V_N 4
#define MATHBITS_V_T float
#define MATHBITS_V_SFX f
#define MATHBITS_V_T_IS_FLOAT
#include "detail/vec_gen.h"

#define MATHBITS_V_N 2
#define MATHBITS_V_T double
#define MATHBITS_V_SFX d
#include "detail/vec_gen.h"

#define MATHBITS_V_N 3
#define MATHBITS_V_T double
#define MATHBITS_V_SFX d
#include "detail/vec_gen.h"

#define MATHBITS_V_N 4
#define MATHBITS_V_T double
#define MATHBITS_V_SFX d
#include "detail/vec_gen.h"

/* Swizzles once vec2 exists */
MATHBITS_INLINE vec2f vec3f_xy(vec3f a) { return vec2f_new(a.x, a.y); }
MATHBITS_INLINE vec2f vec3f_yz(vec3f a) { return vec2f_new(a.y, a.z); }
MATHBITS_INLINE vec2f vec3f_xz(vec3f a) { return vec2f_new(a.x, a.z); }
MATHBITS_INLINE vec2d vec3d_xy(vec3d a) { return vec2d_new(a.x, a.y); }
MATHBITS_INLINE vec2d vec3d_yz(vec3d a) { return vec2d_new(a.y, a.z); }
MATHBITS_INLINE vec2d vec3d_xz(vec3d a) { return vec2d_new(a.x, a.z); }

MATHBITS_INLINE vec3f vec4f_xyz(vec4f a) { return vec3f_new(a.x, a.y, a.z); }
MATHBITS_INLINE vec3d vec4d_xyz(vec4d a) { return vec3d_new(a.x, a.y, a.z); }

#define VEC_XY(v) vec3f_xy(v)
#define VEC_YZ(v) vec3f_yz(v)
#define VEC_XZ(v) vec3f_xz(v)

/* ---- Dynamic views (non-owning) ---- */

typedef struct {
    float *data;
    size_t n;
} vec_f;

typedef struct {
    double *data;
    size_t n;
} vec_d;

MATHBITS_INLINE vec_f vec_view(float *data, size_t n) {
    vec_f v;
    v.data = data;
    v.n = n;
    return v;
}

MATHBITS_INLINE vec_d vec_view_d(double *data, size_t n) {
    vec_d v;
    v.data = data;
    v.n = n;
    return v;
}

MATHBITS_INLINE vec_f vec_alloc(size_t n) {
    vec_f v;
    v.data = (float *)mathbits_malloc(n * sizeof(float));
    v.n = v.data ? n : 0;
    return v;
}

MATHBITS_INLINE vec_d vec_alloc_d(size_t n) {
    vec_d v;
    v.data = (double *)mathbits_malloc(n * sizeof(double));
    v.n = v.data ? n : 0;
    return v;
}

MATHBITS_INLINE void vec_free(vec_f *v) {
    if (!v || !v->data) {
        return;
    }
    mathbits_free(v->data, v->n * sizeof(float));
    v->data = NULL;
    v->n = 0;
}

MATHBITS_INLINE void vec_free_d(vec_d *v) {
    if (!v || !v->data) {
        return;
    }
    mathbits_free(v->data, v->n * sizeof(double));
    v->data = NULL;
    v->n = 0;
}

#define MATHBITS_FVEC_STACK(N, name) \
    float name##_storage[(N)] MATHBITS_ALIGNED(MATHBITS_ALIGN); \
    vec_f name = vec_view(name##_storage, (N))

#define MATHBITS_DVEC_STACK(N, name) \
    double name##_storage[(N)] MATHBITS_ALIGNED(MATHBITS_ALIGN); \
    vec_d name = vec_view_d(name##_storage, (N))

#define MATHBITS_VEC_STACK(T, N, name) T name##_storage[(N)] MATHBITS_ALIGNED(MATHBITS_ALIGN)

MATHBITS_INLINE void vec_fill(vec_f *v, float s) {
    size_t i;
    for (i = 0; i < v->n; ++i) {
        v->data[i] = s;
    }
}

MATHBITS_INLINE void vec_fill_d(vec_d *v, double s) {
    size_t i;
    for (i = 0; i < v->n; ++i) {
        v->data[i] = s;
    }
}

MATHBITS_INLINE void vec_copy(const vec_f *src, vec_f *dst) {
    size_t n = src->n < dst->n ? src->n : dst->n;
    memcpy(dst->data, src->data, n * sizeof(float));
}

MATHBITS_INLINE float vec_dot(const vec_f *a, const vec_f *b) {
    size_t n = a->n < b->n ? a->n : b->n;
    size_t i = 0;
    float acc = 0.0f;
#if MATHBITS_HAS_AVX2
    __m256 vacc = _mm256_setzero_ps();
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a->data + i);
        __m256 vb = _mm256_loadu_ps(b->data + i);
        vacc = _mm256_fmadd_ps(va, vb, vacc);
    }
    float tmp[8];
    _mm256_storeu_ps(tmp, vacc);
    acc = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
#elif MATHBITS_HAS_NEON
    float32x4_t vacc = vdupq_n_f32(0.0f);
    for (; i + 4 <= n; i += 4) {
        float32x4_t va = vld1q_f32(a->data + i);
        float32x4_t vb = vld1q_f32(b->data + i);
        vacc = vmlaq_f32(vacc, va, vb);
    }
    acc = vgetq_lane_f32(vacc, 0) + vgetq_lane_f32(vacc, 1)
        + vgetq_lane_f32(vacc, 2) + vgetq_lane_f32(vacc, 3);
#endif
    for (; i < n; ++i) {
        acc += a->data[i] * b->data[i];
    }
    return acc;
}

MATHBITS_INLINE double vec_dot_d(const vec_d *a, const vec_d *b) {
    size_t n = a->n < b->n ? a->n : b->n;
    size_t i;
    double acc = 0.0;
    for (i = 0; i < n; ++i) {
        acc += a->data[i] * b->data[i];
    }
    return acc;
}

MATHBITS_INLINE void vec_axpy(float alpha, const vec_f *x, vec_f *y) {
    size_t n = x->n < y->n ? x->n : y->n;
    size_t i = 0;
#if MATHBITS_HAS_AVX2
    __m256 va = _mm256_set1_ps(alpha);
    for (; i + 8 <= n; i += 8) {
        __m256 vx = _mm256_loadu_ps(x->data + i);
        __m256 vy = _mm256_loadu_ps(y->data + i);
        _mm256_storeu_ps(y->data + i, _mm256_fmadd_ps(va, vx, vy));
    }
#elif MATHBITS_HAS_NEON
    float32x4_t va = vdupq_n_f32(alpha);
    for (; i + 4 <= n; i += 4) {
        float32x4_t vx = vld1q_f32(x->data + i);
        float32x4_t vy = vld1q_f32(y->data + i);
        vst1q_f32(y->data + i, vmlaq_f32(vy, va, vx));
    }
#endif
    for (; i < n; ++i) {
        y->data[i] += alpha * x->data[i];
    }
}

MATHBITS_INLINE void vec_scale(vec_f *v, float s) {
    size_t i;
    for (i = 0; i < v->n; ++i) {
        v->data[i] *= s;
    }
}

MATHBITS_INLINE float vec_norm_sq(const vec_f *v) {
    return vec_dot(v, v);
}

MATHBITS_INLINE float vec_norm(const vec_f *v) {
    return sqrtf(vec_norm_sq(v));
}

MATHBITS_INLINE float vec_sum(const vec_f *v) {
    size_t i;
    float acc = 0.0f;
    for (i = 0; i < v->n; ++i) {
        acc += v->data[i];
    }
    return acc;
}

MATHBITS_INLINE float vec_min(const vec_f *v) {
    size_t i;
    float m;
    if (v->n == 0) {
        return 0.0f;
    }
    m = v->data[0];
    for (i = 1; i < v->n; ++i) {
        if (v->data[i] < m) {
            m = v->data[i];
        }
    }
    return m;
}

MATHBITS_INLINE float vec_max(const vec_f *v) {
    size_t i;
    float m;
    if (v->n == 0) {
        return 0.0f;
    }
    m = v->data[0];
    for (i = 1; i < v->n; ++i) {
        if (v->data[i] > m) {
            m = v->data[i];
        }
    }
    return m;
}

MATHBITS_INLINE vec_f vec_one_hot(size_t n, size_t idx) {
    vec_f v = vec_alloc(n);
    if (v.data) {
        vec_fill(&v, 0.0f);
        if (idx < n) {
            v.data[idx] = 1.0f;
        }
    }
    return v;
}

/* SoA batch of vec3f — lagrange-style particle loops */
typedef struct {
    float *x, *y, *z;
    size_t n;
} vec3f_batch;

MATHBITS_INLINE vec3f_batch vec3f_batch_view(float *x, float *y, float *z, size_t n) {
    vec3f_batch b;
    b.x = x;
    b.y = y;
    b.z = z;
    b.n = n;
    return b;
}

MATHBITS_INLINE void vec3f_batch_scale(vec3f_batch *b, float s) {
    size_t i;
    for (i = 0; i < b->n; ++i) {
        b->x[i] *= s;
        b->y[i] *= s;
        b->z[i] *= s;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_VEC_H */
