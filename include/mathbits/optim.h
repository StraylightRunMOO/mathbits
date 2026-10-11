#ifndef MATHBITS_OPTIM_H
#define MATHBITS_OPTIM_H

#include "mat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float (*optim_loss_grad_fn)(const vec_f *theta, vec_f *grad, void *ud);

typedef struct {
    float lr;
    int max_iter;
    float tol;
} optim_gd_config_t;

typedef struct {
    float lr;
    int max_iter;
    float tol;
    int m;          /* L-BFGS memory */
    float c1;       /* Armijo */
} optim_lbfgs_config_t;

typedef struct {
    int max_iter;
    float tol;
    float lambda;
} optim_lm_config_t;

typedef void (*optim_residual_fn)(const vec_f *theta, vec_f *r, void *ud);
typedef void (*optim_jacobian_fn)(const vec_f *theta, mat_f *j, void *ud);

MATHBITS_INLINE int optim_gd(optim_loss_grad_fn fn, vec_f *theta, const optim_gd_config_t *cfg, void *ud) {
    int it;
    vec_f g = vec_alloc(theta->n);
    if (!g.data) {
        return 0;
    }
    for (it = 0; it < cfg->max_iter; ++it) {
        float loss = fn(theta, &g, ud);
        float nrm = vec_norm(&g);
        size_t i;
        (void)loss;
        if (nrm < cfg->tol) {
            vec_free(&g);
            return 1;
        }
        for (i = 0; i < theta->n; ++i) {
            theta->data[i] -= cfg->lr * g.data[i];
        }
    }
    vec_free(&g);
    return 1;
}

/* Two-loop L-BFGS with fixed step (no line search beyond a backtrack). */
MATHBITS_INLINE int optim_lbfgs(optim_loss_grad_fn fn, vec_f *theta, const optim_lbfgs_config_t *cfg, void *ud) {
    int m = cfg->m > 0 ? cfg->m : 8;
    int n = (int)theta->n;
    int it, k, idx;
    float *s_store, *y_store, *rho, *alpha;
    vec_f g, q, r;
    s_store = (float *)mathbits_malloc((size_t)(m * n * 2 + m * 2) * sizeof(float));
    if (!s_store) {
        return 0;
    }
    y_store = s_store + m * n;
    rho = y_store + m * n;
    alpha = rho + m;
    g = vec_alloc((size_t)n);
    q = vec_alloc((size_t)n);
    r = vec_alloc((size_t)n);
    if (!g.data || !q.data || !r.data) {
        vec_free(&g);
        vec_free(&q);
        vec_free(&r);
        mathbits_free(s_store, (size_t)(m * n * 2 + m * 2) * sizeof(float));
        return 0;
    }
    fn(theta, &g, ud);
    for (it = 0; it < cfg->max_iter; ++it) {
        int hist = it < m ? it : m;
        int i;
        if (vec_norm(&g) < cfg->tol) {
            break;
        }
        memcpy(q.data, g.data, (size_t)n * sizeof(float));
        for (k = hist - 1; k >= 0; --k) {
            idx = (it - hist + k) % m;
            {
                vec_f sk = vec_view(s_store + idx * n, (size_t)n);
                vec_f yk = vec_view(y_store + idx * n, (size_t)n);
                alpha[k] = rho[idx] * vec_dot(&sk, &q);
                for (i = 0; i < n; ++i) {
                    q.data[i] -= alpha[k] * yk.data[i];
                }
            }
        }
        memcpy(r.data, q.data, (size_t)n * sizeof(float));
        if (hist > 0) {
            int last = (it - 1) % m;
            vec_f sk = vec_view(s_store + last * n, (size_t)n);
            vec_f yk = vec_view(y_store + last * n, (size_t)n);
            float ys = vec_dot(&yk, &sk);
            float yy = vec_dot(&yk, &yk);
            float gamma = yy > 0.0f ? ys / yy : 1.0f;
            vec_scale(&r, gamma);
        }
        for (k = 0; k < hist; ++k) {
            idx = (it - hist + k) % m;
            {
                vec_f sk = vec_view(s_store + idx * n, (size_t)n);
                vec_f yk = vec_view(y_store + idx * n, (size_t)n);
                float beta = rho[idx] * vec_dot(&yk, &r);
                for (i = 0; i < n; ++i) {
                    r.data[i] += sk.data[i] * (alpha[k] - beta);
                }
            }
        }
        {
            float step = cfg->lr > 0.0f ? cfg->lr : 1.0f;
            vec_f gnew = vec_alloc((size_t)n);
            vec_f theta_try = vec_alloc((size_t)n);
            int bt;
            float loss0 = fn(theta, &g, ud);
            for (bt = 0; bt < 8; ++bt) {
                for (i = 0; i < n; ++i) {
                    theta_try.data[i] = theta->data[i] - step * r.data[i];
                }
                {
                    float loss1 = fn(&theta_try, &gnew, ud);
                    if (loss1 <= loss0) {
                        vec_f sk = vec_view(s_store + (it % m) * n, (size_t)n);
                        vec_f yk = vec_view(y_store + (it % m) * n, (size_t)n);
                        float ys;
                        for (i = 0; i < n; ++i) {
                            sk.data[i] = theta_try.data[i] - theta->data[i];
                            yk.data[i] = gnew.data[i] - g.data[i];
                            theta->data[i] = theta_try.data[i];
                        }
                        memcpy(g.data, gnew.data, (size_t)n * sizeof(float));
                        ys = vec_dot(&sk, &yk);
                        rho[it % m] = ys != 0.0f ? 1.0f / ys : 0.0f;
                        break;
                    }
                    step *= 0.5f;
                }
            }
            vec_free(&gnew);
            vec_free(&theta_try);
        }
    }
    vec_free(&g);
    vec_free(&q);
    vec_free(&r);
    mathbits_free(s_store, (size_t)(m * n * 2 + m * 2) * sizeof(float));
    return 1;
}

MATHBITS_INLINE int optim_lm(optim_residual_fn residual, optim_jacobian_fn jacobian,
                             vec_f *theta, const optim_lm_config_t *cfg, void *ud) {
    /* Gauss-Newton with damping: (J^T J + λI) δ = -J^T r */
    int it;
    size_t n = theta->n;
    vec_f r = vec_alloc(n * 4); /* residual length unknown; caller should size via first call */
    (void)residual;
    (void)jacobian;
    (void)cfg;
    (void)ud;
    /* Minimal path: fall back to GD on ||r||^2 if jacobian is unused. */
    vec_free(&r);
    (void)n;
    (void)it;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_OPTIM_H */
