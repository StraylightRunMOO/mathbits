#define MEMENTO_IMPLEMENTATION
#include "memento.h"
#define MATHBITS_USE_MEMENTO
#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include <stdio.h>

int main(void) {
    mathbits_allocator_t a;
    mat_f A, B, C;
    memento_init();
    a = mathbits_memento_allocator(memento_thread_heap_get());
    mathbits_set_alloc(&a);
    A = mat_alloc(8, 8);
    B = mat_alloc(8, 8);
    C = mat_alloc(8, 8);
    mat_set_identity(&A);
    mat_set_identity(&B);
    mat_gemm(1.0f, &A, &B, 0.0f, &C);
    printf("C[0,0] = %.1f (memento gemm)\n", mat_get(&C, 0, 0));
    mat_free(&A);
    mat_free(&B);
    mat_free(&C);
    memento_shutdown();
    return 0;
}
