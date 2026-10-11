#include "mathbits/vec.h"
#include <stdio.h>

int main(void) {
    vec3f a = vec3f_new(1.0f, 2.0f, 3.0f);
    vec3f b = vec3f_add(a, vec3f_splat(1.0f));
    if (b.x < 1.5f) {
        printf("FAIL vec-only\n");
        return 1;
    }
    printf("PASS vec-only include\n");
    return 0;
}
