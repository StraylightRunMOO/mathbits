#ifndef MATHBITS_GEOMETRY_H
#define MATHBITS_GEOMETRY_H

#include "vec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Quaternion is layout-compatible with lagrange lg_quat_t: x,y,z,w float. */
typedef struct {
    float x, y, z, w;
} quat_f;

typedef struct {
    double x, y, z, w;
} quat_d;

typedef struct {
    vec3f t;
    quat_f q;
} rigid_f;

typedef struct {
    vec3f min, max;
} aabb_f;

typedef struct {
    vec3f c;
    float r;
} sphere_f;

MATHBITS_INLINE quat_f quat_new(float x, float y, float z, float w) {
    quat_f q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

MATHBITS_INLINE quat_f quat_identity(void) { return quat_new(0.0f, 0.0f, 0.0f, 1.0f); }

MATHBITS_INLINE quat_f quat_mul(quat_f a, quat_f b) {
    return quat_new(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

MATHBITS_INLINE quat_f quat_conj(quat_f q) { return quat_new(-q.x, -q.y, -q.z, q.w); }

MATHBITS_INLINE float quat_norm_sq(quat_f q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

MATHBITS_INLINE quat_f quat_normalize(quat_f q) {
    float n = sqrtf(quat_norm_sq(q));
    if (n < 1e-8f) {
        return quat_identity();
    }
    n = 1.0f / n;
    return quat_new(q.x * n, q.y * n, q.z * n, q.w * n);
}

MATHBITS_INLINE quat_f quat_from_axis_angle(vec3f axis, float angle) {
    float h = angle * 0.5f;
    float s = sinf(h);
    vec3f n = vec3f_normalize(axis);
    return quat_normalize(quat_new(n.x * s, n.y * s, n.z * s, cosf(h)));
}

MATHBITS_INLINE vec3f quat_rotate(quat_f q, vec3f v) {
    quat_f p = quat_new(v.x, v.y, v.z, 0.0f);
    quat_f r = quat_mul(quat_mul(q, p), quat_conj(q));
    return vec3f_new(r.x, r.y, r.z);
}

MATHBITS_INLINE quat_f quat_slerp(quat_f a, quat_f b, float t) {
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    quat_f bb = b;
    if (dot < 0.0f) {
        dot = -dot;
        bb = quat_new(-b.x, -b.y, -b.z, -b.w);
    }
    if (dot > 0.9995f) {
        return quat_normalize(quat_new(
            a.x + t * (bb.x - a.x),
            a.y + t * (bb.y - a.y),
            a.z + t * (bb.z - a.z),
            a.w + t * (bb.w - a.w)));
    }
    {
        float th = acosf(num_clamp_f(dot, -1.0f, 1.0f));
        float s = sinf(th);
        float w1 = sinf((1.0f - t) * th) / s;
        float w2 = sinf(t * th) / s;
        return quat_new(w1 * a.x + w2 * bb.x, w1 * a.y + w2 * bb.y,
                        w1 * a.z + w2 * bb.z, w1 * a.w + w2 * bb.w);
    }
}

MATHBITS_INLINE rigid_f rigid_from_translation_rotation(vec3f t, quat_f q) {
    rigid_f r;
    r.t = t;
    r.q = q;
    return r;
}

MATHBITS_INLINE vec3f rigid_transform_point(rigid_f t, vec3f p) {
    return vec3f_add(quat_rotate(t.q, p), t.t);
}

MATHBITS_INLINE vec3f rigid_transform_vector(rigid_f t, vec3f v) {
    return quat_rotate(t.q, v);
}

MATHBITS_INLINE rigid_f rigid_inverse(rigid_f t) {
    rigid_f r;
    r.q = quat_conj(t.q);
    r.t = vec3f_neg(quat_rotate(r.q, t.t));
    return r;
}

MATHBITS_INLINE rigid_f rigid_mul(rigid_f a, rigid_f b) {
    rigid_f r;
    r.q = quat_mul(a.q, b.q);
    r.t = vec3f_add(quat_rotate(a.q, b.t), a.t);
    return r;
}

MATHBITS_INLINE aabb_f aabb_from_points(const vec3f *pts, size_t n) {
    aabb_f b;
    size_t i;
    b.min = b.max = pts[0];
    for (i = 1; i < n; ++i) {
        b.min = vec3f_min(b.min, pts[i]);
        b.max = vec3f_max(b.max, pts[i]);
    }
    return b;
}

MATHBITS_INLINE int aabb_intersects(const aabb_f *a, const aabb_f *b) {
    return a->min.x <= b->max.x && a->max.x >= b->min.x
        && a->min.y <= b->max.y && a->max.y >= b->min.y
        && a->min.z <= b->max.z && a->max.z >= b->min.z;
}

MATHBITS_INLINE sphere_f sphere_new(vec3f c, float r) {
    sphere_f s;
    s.c = c;
    s.r = r;
    return s;
}

MATHBITS_INLINE void mathbits_integrate_euler(vec_f *pos, vec_f *vel, const vec_f *acc, float dt) {
    size_t i, n = pos->n;
    if (vel->n < n) {
        n = vel->n;
    }
    if (acc->n < n) {
        n = acc->n;
    }
    for (i = 0; i < n; ++i) {
        vel->data[i] += acc->data[i] * dt;
        pos->data[i] += vel->data[i] * dt;
    }
}

MATHBITS_INLINE void mathbits_integrate_verlet(vec_f *pos, vec_f *vel, const vec_f *acc, float dt) {
    size_t i, n = pos->n;
    if (vel->n < n) {
        n = vel->n;
    }
    if (acc->n < n) {
        n = acc->n;
    }
    for (i = 0; i < n; ++i) {
        pos->data[i] += vel->data[i] * dt + 0.5f * acc->data[i] * dt * dt;
        vel->data[i] += acc->data[i] * dt;
    }
}

MATHBITS_INLINE vec3f vec3f_integrate_verlet(vec3f pos, vec3f vel, vec3f acc, float dt) {
    return vec3f_add(pos, vec3f_add(vec3f_scale(vel, dt), vec3f_scale(acc, 0.5f * dt * dt)));
}

typedef void (*vec_deriv_fn)(const vec_f *y, float t, vec_f *dydt, void *ud);

MATHBITS_INLINE void mathbits_integrate_rk4(vec_f *y, vec_deriv_fn deriv, float t, float dt, void *ud) {
    size_t n = y->n, i;
    float *buf = (float *)mathbits_malloc(n * 5 * sizeof(float));
    vec_f k1, k2, k3, k4, yt;
    if (!buf) {
        return;
    }
    k1 = vec_view(buf, n);
    k2 = vec_view(buf + n, n);
    k3 = vec_view(buf + 2 * n, n);
    k4 = vec_view(buf + 3 * n, n);
    yt = vec_view(buf + 4 * n, n);
    deriv(y, t, &k1, ud);
    for (i = 0; i < n; ++i) {
        yt.data[i] = y->data[i] + 0.5f * dt * k1.data[i];
    }
    deriv(&yt, t + 0.5f * dt, &k2, ud);
    for (i = 0; i < n; ++i) {
        yt.data[i] = y->data[i] + 0.5f * dt * k2.data[i];
    }
    deriv(&yt, t + 0.5f * dt, &k3, ud);
    for (i = 0; i < n; ++i) {
        yt.data[i] = y->data[i] + dt * k3.data[i];
    }
    deriv(&yt, t + dt, &k4, ud);
    for (i = 0; i < n; ++i) {
        y->data[i] += (dt / 6.0f) * (k1.data[i] + 2.0f * k2.data[i] + 2.0f * k3.data[i] + k4.data[i]);
    }
    mathbits_free(buf, n * 5 * sizeof(float));
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_GEOMETRY_H */
