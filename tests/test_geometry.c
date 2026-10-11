#include "mathbits/mathbits.h"
#include "test_common.h"

int test_geometry(void) {
    quat_f q = quat_from_axis_angle(vec3f_new(0, 1, 0), (float)MATHBITS_PI_2);
    vec3f v = vec3f_new(1, 0, 0);
    vec3f r = quat_rotate(q, v);
    rigid_f T, Ti;
    vec3f w;
    ASSERT_NEAR(r.x, 0.0f, 1e-5);
    ASSERT_NEAR(r.y, 0.0f, 1e-5);
    ASSERT_NEAR(r.z, -1.0f, 1e-5); /* right-handed: x --+90 y--> -z */

    T = rigid_from_translation_rotation(vec3f_new(1, 2, 3), quat_identity());
    w = rigid_transform_point(T, vec3f_new(0, 0, 0));
    ASSERT_NEAR(w.x, 1.0f, 1e-6);
    Ti = rigid_inverse(T);
    w = rigid_transform_point(rigid_mul(Ti, T), vec3f_new(4, 5, 6));
    ASSERT_NEAR(w.x, 4.0f, 1e-4);
    ASSERT_NEAR(w.y, 5.0f, 1e-4);
    ASSERT_NEAR(w.z, 6.0f, 1e-4);

    {
        vec3f pts[2] = {vec3f_new(0, 0, 0), vec3f_new(1, 2, 3)};
        aabb_f box = aabb_from_points(pts, 2);
        ASSERT_NEAR(box.max.z, 3.0f, 1e-6);
    }
    printf("PASS test_geometry\n");
    return 0;
}
