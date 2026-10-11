#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include <stdio.h>

/* Landmark-style MLP: 16 -> 64 -> 32 -> 8 */
int main(void) {
    enum { IN = 16, H1 = 64, H2 = 32, OUT = 8 };
    float w0[H1 * IN], b0[H1], w1[H2 * H1], b1[H2], w2[OUT * H2], b2[OUT];
    float feat[IN], pred[OUT];
    nn_layer_t layers[3];
    nn_sequential_t net;
    nn_workspace_t ws;
    vec_f in, out;
    int i;

    for (i = 0; i < H1 * IN; ++i) {
        w0[i] = 0.01f;
    }
    for (i = 0; i < H1; ++i) {
        b0[i] = 0.0f;
    }
    for (i = 0; i < H2 * H1; ++i) {
        w1[i] = 0.01f;
    }
    for (i = 0; i < H2; ++i) {
        b1[i] = 0.0f;
    }
    for (i = 0; i < OUT * H2; ++i) {
        w2[i] = 0.01f;
    }
    for (i = 0; i < OUT; ++i) {
        b2[i] = 0.0f;
    }
    for (i = 0; i < IN; ++i) {
        feat[i] = 0.1f * (float)i;
    }

    layers[0] = nn_dense(IN, H1, w0, b0, NN_RELU);
    layers[1] = nn_dense(H1, H2, w1, b1, NN_GELU);
    layers[2] = nn_dense(H2, OUT, w2, b2, NN_NONE);
    net = nn_sequential(layers, 3);
    ws = nn_workspace_alloc(&net);
    in = vec_view(feat, IN);
    out = vec_view(pred, OUT);
    if (!nn_forward(&net, &in, &out, &ws)) {
        fprintf(stderr, "forward failed\n");
        return 1;
    }
    printf("mlp out:");
    for (i = 0; i < OUT; ++i) {
        printf(" %.4f", pred[i]);
    }
    printf("\n");
    nn_workspace_free(&ws);
    return 0;
}
