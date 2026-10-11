#ifndef MATHBITS_NN_H
#define MATHBITS_NN_H

#include "mat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NN_NONE = 0,
    NN_RELU,
    NN_GELU,
    NN_SIGMOID,
    NN_TANH,
    NN_SOFTMAX
} nn_act_t;

typedef struct {
    int in_dim;
    int out_dim;
    const float *w; /* out × in, row-major */
    const float *b; /* out, or NULL */
    nn_act_t act;
} nn_layer_t;

typedef struct {
    nn_layer_t *layers;
    int n_layers;
} nn_sequential_t;

typedef struct {
    float *buf;
    size_t cap;
} nn_workspace_t;

MATHBITS_INLINE nn_layer_t nn_dense(int in_dim, int out_dim, const float *w, const float *b, nn_act_t act) {
    nn_layer_t l;
    l.in_dim = in_dim;
    l.out_dim = out_dim;
    l.w = w;
    l.b = b;
    l.act = act;
    return l;
}

MATHBITS_INLINE nn_sequential_t nn_sequential(nn_layer_t *layers, int n) {
    nn_sequential_t s;
    s.layers = layers;
    s.n_layers = n;
    return s;
}

MATHBITS_INLINE size_t nn_workspace_bytes(const nn_sequential_t *net) {
    int i;
    size_t maxd = 0;
    for (i = 0; i < net->n_layers; ++i) {
        size_t d = (size_t)net->layers[i].out_dim;
        if (d > maxd) {
            maxd = d;
        }
    }
    /* two activation buffers */
    return maxd * 2 * sizeof(float);
}

MATHBITS_INLINE nn_workspace_t nn_workspace_alloc(const nn_sequential_t *net) {
    nn_workspace_t ws;
    ws.cap = nn_workspace_bytes(net);
    ws.buf = (float *)mathbits_malloc(ws.cap);
    return ws;
}

MATHBITS_INLINE void nn_workspace_free(nn_workspace_t *ws) {
    if (!ws || !ws->buf) {
        return;
    }
    mathbits_free(ws->buf, ws->cap);
    ws->buf = NULL;
    ws->cap = 0;
}

MATHBITS_INLINE float nn_gelu(float x) {
    /* tanh approximation */
    float c = 0.044715f;
    float u = MATHBITS_SQRT2_PI * (x + c * x * x * x);
    return 0.5f * x * (1.0f + tanhf(u));
}

MATHBITS_INLINE void nn_activate(nn_act_t act, float *x, int n) {
    int i;
    switch (act) {
    case NN_RELU:
        for (i = 0; i < n; ++i) {
            if (x[i] < 0.0f) {
                x[i] = 0.0f;
            }
        }
        break;
    case NN_GELU:
        for (i = 0; i < n; ++i) {
            x[i] = nn_gelu(x[i]);
        }
        break;
    case NN_SIGMOID:
        for (i = 0; i < n; ++i) {
            x[i] = 1.0f / (1.0f + expf(-x[i]));
        }
        break;
    case NN_TANH:
        for (i = 0; i < n; ++i) {
            x[i] = tanhf(x[i]);
        }
        break;
    case NN_SOFTMAX: {
        float m = x[0], s = 0.0f;
        for (i = 1; i < n; ++i) {
            if (x[i] > m) {
                m = x[i];
            }
        }
        for (i = 0; i < n; ++i) {
            x[i] = expf(x[i] - m);
            s += x[i];
        }
        for (i = 0; i < n; ++i) {
            x[i] /= s;
        }
        break;
    }
    case NN_NONE:
    default:
        break;
    }
}

MATHBITS_INLINE void nn_forward_layer(const nn_layer_t *l, const vec_f *in, vec_f *out) {
    mat_f w = mat_view((float *)(uintptr_t)l->w, (size_t)l->out_dim, (size_t)l->in_dim, (size_t)l->in_dim);
    int i;
    vec_fill(out, 0.0f);
    mat_gemv(1.0f, &w, in, 0.0f, out);
    if (l->b) {
        for (i = 0; i < l->out_dim && i < (int)out->n; ++i) {
            out->data[i] += l->b[i];
        }
    }
    nn_activate(l->act, out->data, l->out_dim);
}

MATHBITS_INLINE int nn_forward(const nn_sequential_t *net, const vec_f *in, vec_f *out, nn_workspace_t *ws) {
    int i;
    vec_f cur = *in;
    float *a = ws->buf;
    float *b = ws->buf + (ws->cap / (2 * sizeof(float)));
    size_t half = ws->cap / (2 * sizeof(float));
    for (i = 0; i < net->n_layers; ++i) {
        nn_layer_t *l = &net->layers[i];
        vec_f dst;
        float *slot = (i % 2 == 0) ? a : b;
        if ((size_t)l->out_dim > half) {
            return 0;
        }
        dst = vec_view(slot, (size_t)l->out_dim);
        nn_forward_layer(l, &cur, &dst);
        cur = dst;
    }
    {
        size_t n = cur.n < out->n ? cur.n : out->n;
        memcpy(out->data, cur.data, n * sizeof(float));
    }
    return 1;
}

MATHBITS_INLINE int nn_forward_batch(const nn_sequential_t *net, const mat_f *in, mat_f *out, nn_workspace_t *ws) {
    size_t r;
    for (r = 0; r < in->rows; ++r) {
        vec_f xin = vec_view(in->data + r * in->ld, in->cols);
        vec_f xout = vec_view(out->data + r * out->ld, out->cols);
        if (!nn_forward(net, &xin, &xout, ws)) {
            return 0;
        }
    }
    return 1;
}

MATHBITS_INLINE int nn_load_weights_flat(nn_sequential_t *net, const float *blob, size_t n) {
    size_t off = 0;
    int i, j;
    for (i = 0; i < net->n_layers; ++i) {
        nn_layer_t *l = &net->layers[i];
        size_t nw = (size_t)l->in_dim * (size_t)l->out_dim;
        size_t nb = (size_t)l->out_dim;
        if (off + nw + nb > n) {
            return 0;
        }
        /* The layer already points at caller storage; copy into it. */
        if (l->w) {
            memcpy((void *)(uintptr_t)l->w, blob + off, nw * sizeof(float));
        }
        off += nw;
        if (l->b) {
            memcpy((void *)(uintptr_t)l->b, blob + off, nb * sizeof(float));
        }
        off += nb;
        (void)j;
    }
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_NN_H */
