#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include "test_common.h"

int main(void) {
    int rc = 0;
    rc |= test_num();
    rc |= test_vec();
    rc |= test_mat();
    rc |= test_linalg();
    rc |= test_tensor();
    rc |= test_nn();
    rc |= test_geometry();
    rc |= test_optim();
    rc |= test_sparse();
    if (rc == 0) {
        printf("all tests passed\n");
    }
    return rc;
}
