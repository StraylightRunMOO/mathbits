#include "mathbits/mathbits.h"
#include "test_common.h"

static float quad_loss(const vec_f *theta, vec_f *grad, void *ud) {
    /* f = 0.5 (x-1)^2 + 0.5 (y+2)^2 */
    (void)ud;
    grad->data[0] = theta->data[0] - 1.0f;
    grad->data[1] = theta->data[1] + 2.0f;
    return 0.5f * grad->data[0] * grad->data[0] + 0.5f * grad->data[1] * grad->data[1];
}

int test_optim(void) {
    float th[2] = {0.0f, 0.0f};
    vec_f theta = vec_view(th, 2);
    optim_gd_config_t cfg;
    cfg.lr = 0.2f;
    cfg.max_iter = 80;
    cfg.tol = 1e-5f;
    ASSERT(optim_gd(quad_loss, &theta, &cfg, NULL));
    ASSERT_NEAR(theta.data[0], 1.0f, 1e-3);
    ASSERT_NEAR(theta.data[1], -2.0f, 1e-3);

    th[0] = 0;
    th[1] = 0;
    {
        optim_lbfgs_config_t lc;
        lc.lr = 1.0f;
        lc.max_iter = 40;
        lc.tol = 1e-6f;
        lc.m = 5;
        lc.c1 = 1e-4f;
        ASSERT(optim_lbfgs(quad_loss, &theta, &lc, NULL));
        ASSERT_NEAR(theta.data[0], 1.0f, 1e-2);
        ASSERT_NEAR(theta.data[1], -2.0f, 1e-2);
    }
    printf("PASS test_optim\n");
    return 0;
}
