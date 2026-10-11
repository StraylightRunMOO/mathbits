#ifndef MATHBITS_SAMPLERS_METROPOLIS_H
#define MATHBITS_SAMPLERS_METROPOLIS_H

#include "gibbs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float (*mathbits_log_target_fn)(const vec_f *x, void *ud);
typedef void (*mathbits_proposal_fn)(const vec_f *x, vec_f *y, uint32_t *rng, void *ud);

MATHBITS_INLINE void metropolis_hastings(mathbits_log_target_fn log_target,
                                         mathbits_proposal_fn proposal,
                                         vec_f *state, int n_steps,
                                         uint32_t *rng_state, void *ud) {
    vec_f prop = vec_alloc(state->n);
    int t;
    float logp = log_target(state, ud);
    if (!prop.data) {
        return;
    }
    for (t = 0; t < n_steps; ++t) {
        float logq, u, a;
        proposal(state, &prop, rng_state, ud);
        logq = log_target(&prop, ud);
        a = logq - logp;
        u = logf((mathbits_splitmix32(rng_state) + 1.0f) / 4294967296.0f);
        if (a >= 0.0f || u < a) {
            memcpy(state->data, prop.data, state->n * sizeof(float));
            logp = logq;
        }
    }
    vec_free(&prop);
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_SAMPLERS_METROPOLIS_H */
