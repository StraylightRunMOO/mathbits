#include "mathbits/mathbits.h"
#include "test_common.h"

int test_tensor(void) {
    float data[2 * 3 * 4];
    size_t shape[3] = {2, 3, 4};
    tensor_f t;
    mat_f collapsed;
    float cbuf[12];
    size_t idx[3] = {1, 2, 0};
    size_t i;
    for (i = 0; i < 24; ++i) {
        data[i] = (float)i;
    }
    t = tensor_view(data, shape, 3);
    ASSERT_NEAR(tensor_get(&t, idx), data[1 * 12 + 2 * 4 + 0], 1e-6);
    collapsed = mat_view(cbuf, 3, 4, 4);
    tensor_sum_axis(&t, 0, &collapsed);
    ASSERT_NEAR(mat_get(&collapsed, 0, 0), data[0] + data[12], 1e-5);
    printf("PASS test_tensor\n");
    return 0;
}
