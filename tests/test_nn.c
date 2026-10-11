#include "mathbits/mathbits.h"
#include "test_common.h"

int test_nn(void) {
    /* y = relu(W x + b) with W = 2x2 identity, b = 0, x = (-1, 2) -> (0, 2) */
    float w0[] = {1, 0, 0, 1};
    float b0[] = {0, 0};
    float w1[] = {1, 1};
    float b1[] = {0};
    float inb[] = {-1.0f, 2.0f};
    float outb[1];
    nn_layer_t layers[2];
    nn_sequential_t net;
    nn_workspace_t ws;
    vec_f in, out;
    layers[0] = nn_dense(2, 2, w0, b0, NN_RELU);
    layers[1] = nn_dense(2, 1, w1, b1, NN_NONE);
    net = nn_sequential(layers, 2);
    ws = nn_workspace_alloc(&net);
    ASSERT(ws.buf != NULL);
    in = vec_view(inb, 2);
    out = vec_view(outb, 1);
    ASSERT(nn_forward(&net, &in, &out, &ws));
    ASSERT_NEAR(out.data[0], 2.0f, 1e-5);

    {
        float bin[4] = {-1, 2, 3, -4};
        float bout[2];
        mat_f bi = mat_view(bin, 2, 2, 2);
        mat_f bo = mat_view(bout, 2, 1, 1);
        ASSERT(nn_forward_batch(&net, &bi, &bo, &ws));
        ASSERT_NEAR(bout[0], 2.0f, 1e-5);
        /* second row x=(3,-4) -> relu (3,0) -> 3 */
        ASSERT_NEAR(bout[1], 3.0f, 1e-5);
    }
    nn_workspace_free(&ws);
    printf("PASS test_nn\n");
    return 0;
}
