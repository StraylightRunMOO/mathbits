/*
 * mathbits - num.h
 * Scalar helpers, constants, safe arithmetic.
 * Does not reimplement SIGMA special functions.
 */

#ifndef MATHBITS_NUM_H
#define MATHBITS_NUM_H

#include "config.h"
#include "alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATHBITS_PI        3.14159265358979323846
#define MATHBITS_2PI       6.28318530717958647693
#define MATHBITS_PI_2      1.57079632679489661923
#define MATHBITS_1_PI      0.31830988618379067154
#define MATHBITS_SQRT2     1.41421356237309504880
#define MATHBITS_LN2       0.69314718055994530942
#define MATHBITS_E         2.71828182845904523536
#define MATHBITS_SQRT2_PI  0.79788456080286535588  /* sqrt(2/pi) for GELU */

MATHBITS_INLINE float num_eps_f(void) { return FLT_EPSILON; }
MATHBITS_INLINE double num_eps_d(void) { return DBL_EPSILON; }

#define num_eps(T) _Generic((T)0, float: num_eps_f(), default: num_eps_d())

MATHBITS_INLINE float num_clamp_f(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

MATHBITS_INLINE double num_clamp_d(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

MATHBITS_INLINE float num_clamp(float x, float lo, float hi) {
    return num_clamp_f(x, lo, hi);
}

MATHBITS_INLINE float num_sign_f(float x) {
    return (x > 0.0f) - (x < 0.0f);
}

MATHBITS_INLINE double num_sign_d(double x) {
    return (double)((x > 0.0) - (x < 0.0));
}

MATHBITS_INLINE float num_sign(float x) { return num_sign_f(x); }

MATHBITS_INLINE int num_almost_equal_f(float a, float b, float tol) {
    float d = a - b;
    if (d < 0.0f) {
        d = -d;
    }
    return d <= tol;
}

MATHBITS_INLINE int num_almost_equal_d(double a, double b, double tol) {
    double d = a - b;
    if (d < 0.0) {
        d = -d;
    }
    return d <= tol;
}

MATHBITS_INLINE int num_almost_equal(float a, float b, float tol) {
    return num_almost_equal_f(a, b, tol);
}

MATHBITS_INLINE float num_hypot_f(float a, float b) { return hypotf(a, b); }
MATHBITS_INLINE double num_hypot_d(double a, double b) { return hypot(a, b); }

MATHBITS_INLINE float num_safe_div_f(float a, float b, float fallback) {
    return (b == 0.0f || !isfinite(b)) ? fallback : a / b;
}

MATHBITS_INLINE double num_safe_div_d(double a, double b, double fallback) {
    return (b == 0.0 || !isfinite(b)) ? fallback : a / b;
}

MATHBITS_INLINE float num_sq_f(float x) { return x * x; }
MATHBITS_INLINE double num_sq_d(double x) { return x * x; }

MATHBITS_INLINE float num_abs_f(float x) { return fabsf(x); }
MATHBITS_INLINE double num_abs_d(double x) { return fabs(x); }

MATHBITS_INLINE float num_min_f(float a, float b) { return a < b ? a : b; }
MATHBITS_INLINE float num_max_f(float a, float b) { return a > b ? a : b; }
MATHBITS_INLINE double num_min_d(double a, double b) { return a < b ? a : b; }
MATHBITS_INLINE double num_max_d(double a, double b) { return a > b ? a : b; }

MATHBITS_INLINE int num_is_finite_f(float x) { return isfinite(x); }
MATHBITS_INLINE int num_is_finite_d(double x) { return isfinite(x); }

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_NUM_H */
