#ifndef MATHBITS_SAMPLERS_GIBBS_H
#define MATHBITS_SAMPLERS_GIBBS_H

#include "../mat/transition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*mathbits_rng32_fn)(void *rng);

MATHBITS_INLINE uint32_t mathbits_splitmix32(uint32_t *s) {
    uint32_t z = (*s += 0x9e3779b9u);
    z = (z ^ (z >> 16)) * 0x85ebca6bu;
    z = (z ^ (z >> 13)) * 0xc2b2ae35u;
    return z ^ (z >> 16);
}

/* Discrete Gibbs / Markov step on a row-stochastic P. state is a one-hot or
 * probability vector; we sample an index and write a one-hot. */
MATHBITS_INLINE void gibbs_chain(const transition_f *p, int n_steps, vec_f *state, uint32_t *rng_state) {
    size_t n = p->rows;
    int t;
    size_t i, cur = 0;
    float best = -1.0f;
    for (i = 0; i < n && i < state->n; ++i) {
        if (state->data[i] > best) {
            best = state->data[i];
            cur = i;
        }
    }
    for (t = 0; t < n_steps; ++t) {
        float u = (mathbits_splitmix32(rng_state) / 4294967296.0f);
        float acc = 0.0f;
        size_t nxt = n - 1;
        for (i = 0; i < p->cols; ++i) {
            acc += mat_get(p, cur, i);
            if (u <= acc) {
                nxt = i;
                break;
            }
        }
        cur = nxt;
    }
    vec_fill(state, 0.0f);
    if (cur < state->n) {
        state->data[cur] = 1.0f;
    }
    (void)n;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_SAMPLERS_GIBBS_H */
