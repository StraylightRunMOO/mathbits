#include "mathbits/mathbits.h"
#include "test_common.h"

int test_num(void) {
    ASSERT_NEAR(num_clamp(1.5f, 0.0f, 1.0f), 1.0f, 1e-7);
    ASSERT_NEAR(num_clamp(-1.0f, 0.0f, 1.0f), 0.0f, 1e-7);
    ASSERT(num_almost_equal(1.0f, 1.0f + 1e-8f, 1e-6f));
    ASSERT(num_sign(3.0f) == 1.0f);
    ASSERT(num_sign(-2.0f) == -1.0f);
    ASSERT_NEAR(num_safe_div_f(1.0f, 0.0f, 42.0f), 42.0f, 1e-7);
    ASSERT_NEAR(num_eps_f(), (float)FLT_EPSILON, 0.0);
    printf("PASS test_num\n");
    return 0;
}
