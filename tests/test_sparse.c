#include "mathbits/mathbits.h"
#include "mathbits/mat/adjacency.h"
#include "mathbits/mat/transition.h"
#include "mathbits/samplers/gibbs.h"
#include "test_common.h"

int test_sparse(void) {
    uint32_t rows[] = {0, 1, 2};
    uint32_t cols[] = {0, 1, 0};
    float vals[] = {2.0f, 3.0f, 4.0f};
    sparse_mat_f s = sparse_from_triplets(rows, cols, vals, 3, 3, 3);
    float xb[3] = {1, 1, 1};
    float yb[3] = {0, 0, 0};
    vec_f x = vec_view(xb, 3);
    vec_f y = vec_view(yb, 3);
    uint32_t edges[][2] = {{0, 1}, {1, 2}, {2, 0}};
    adjacency_f adj;
    transition_f P;
    ASSERT(s.vals != NULL);
    sparse_gemv(1.0f, &s, &x, 0.0f, &y);
    ASSERT_NEAR(y.data[0], 2.0f, 1e-5);
    ASSERT_NEAR(y.data[1], 3.0f, 1e-5);
    ASSERT_NEAR(y.data[2], 4.0f, 1e-5);
    sparse_free(&s);

    adj = adjacency_from_edges(edges, 3, 3);
    P = transition_row_stochastic(&adj);
    ASSERT_NEAR(mat_get(&P, 0, 0) + mat_get(&P, 0, 1) + mat_get(&P, 0, 2), 1.0f, 1e-5);
    {
        float pi[3];
        vec_f v = vec_view(pi, 3);
        ASSERT(transition_stationary(&P, &v, 50, 1e-6f));
        ASSERT_NEAR(pi[0] + pi[1] + pi[2], 1.0f, 1e-3);
    }
    sparse_free(&adj);
    mat_free(&P);
    printf("PASS test_sparse\n");
    return 0;
}
