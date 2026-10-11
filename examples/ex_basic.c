#define MATHBITS_IMPLEMENTATION
#include "mathbits/mathbits.h"
#include <stdio.h>

int main(void) {
    vec3f a = vec3f_new(1.0f, 2.0f, 3.0f);
    vec3f b = vec3f_new(0.0f, 1.0f, 0.0f);
    vec3f c = vec3f_cross(a, b);
    mat3f I = mat3f_identity();
    vec3f d = mat3f_mul_vec(I, a);
    printf("cross = (%.3f, %.3f, %.3f)\n", c.x, c.y, c.z);
    printf("I*a   = (%.3f, %.3f, %.3f)\n", d.x, d.y, d.z);
    return 0;
}
