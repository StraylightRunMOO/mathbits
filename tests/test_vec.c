#include "mathbits/mathbits.h"
#include "test_common.h"

int test_vec(void) {
    vec3f a = vec3f_new(1.0f, 2.0f, 3.0f);
    vec3f b = vec3f_splat(4.0f);
    vec3f c = vec3f_add(a, b);
    vec3f cr;
    float buf[8];
    vec_f v;
    ASSERT_NEAR(c.x, 5.0f, 1e-6);
    ASSERT_NEAR(c.y, 6.0f, 1e-6);
    ASSERT_NEAR(c.z, 7.0f, 1e-6);
    ASSERT_NEAR(vec3f_dot(a, b), 24.0f, 1e-5);
    cr = vec3f_cross(vec3f_new(1, 0, 0), vec3f_new(0, 1, 0));
    ASSERT_NEAR(cr.x, 0.0f, 1e-6);
    ASSERT_NEAR(cr.y, 0.0f, 1e-6);
    ASSERT_NEAR(cr.z, 1.0f, 1e-6);
    ASSERT_NEAR(vec3f_norm(vec3f_new(3, 4, 0)), 5.0f, 1e-5);
    ASSERT_NEAR(vec3f_xy(a).x, 1.0f, 1e-6);
    ASSERT_NEAR(vec3f_xy(a).y, 2.0f, 1e-6);

    v = vec_view(buf, 8);
    vec_fill(&v, 1.0f);
    ASSERT_NEAR(vec_sum(&v), 8.0f, 1e-5);
    vec_axpy(2.0f, &v, &v);
    ASSERT_NEAR(v.data[0], 3.0f, 1e-5);
    ASSERT_NEAR(vec_dot(&v, &v), 9.0f * 8.0f, 1e-3);

    {
        vec3d d = vec3d_new(1.0, 0.0, 0.0);
        ASSERT_NEAR(vec3d_norm(d), 1.0, 1e-12);
    }
    printf("PASS test_vec\n");
    return 0;
}
