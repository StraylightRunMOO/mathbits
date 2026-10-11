/* Optional: core mathbits stays single-threaded. This example shows
 * a batch of gemm work the caller could farm out with suspenders.h. */
#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include <stdio.h>

int main(void) {
    float a[16], b[16], c[16];
    mat_f A = mat_view(a, 4, 4, 4);
    mat_f B = mat_view(b, 4, 4, 4);
    mat_f C = mat_view(c, 4, 4, 4);
    mat_set_identity(&A);
    mat_set_identity(&B);
    mat_gemm(1.0f, &A, &B, 0.0f, &C);
    printf("batch gemm C[0,0]=%.1f\n", mat_get(&C, 0, 0));
    return 0;
}
