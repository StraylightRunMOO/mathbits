/*
 * Included repeatedly from vec.h. Requires:
 *   MATHBITS_V_N     dimension (integer literal)
 *   MATHBITS_V_T     scalar type (float or double)
 *   MATHBITS_V_SFX   suffix (f or d)
 */

#ifndef MATHBITS_V_N
#  error "MATHBITS_V_N must be defined before including vec_gen.h"
#endif
#ifndef MATHBITS_V_T
#  error "MATHBITS_V_T must be defined before including vec_gen.h"
#endif
#ifndef MATHBITS_V_SFX
#  error "MATHBITS_V_SFX must be defined before including vec_gen.h"
#endif

#define MB_VN MATHBITS_PASTE3(vec, MATHBITS_V_N, MATHBITS_V_SFX)

typedef struct MB_VN {
#if MATHBITS_V_N == 2
    MATHBITS_V_T x, y;
#elif MATHBITS_V_N == 3
    MATHBITS_V_T x, y, z;
#elif MATHBITS_V_N == 4
    MATHBITS_V_T x, y, z, w;
#else
    MATHBITS_V_T v[MATHBITS_V_N];
#endif
} MB_VN;

MATHBITS_INLINE MATHBITS_V_T *MATHBITS_PASTE2(MB_VN, _ptr)(MB_VN *a) {
#if MATHBITS_V_N >= 2 && MATHBITS_V_N <= 4
    return &a->x;
#else
    return a->v;
#endif
}

MATHBITS_INLINE const MATHBITS_V_T *MATHBITS_PASTE2(MB_VN, _cptr)(const MB_VN *a) {
#if MATHBITS_V_N >= 2 && MATHBITS_V_N <= 4
    return &a->x;
#else
    return a->v;
#endif
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _zero)(void) {
    MB_VN r;
    MATHBITS_V_T *p = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        p[i] = (MATHBITS_V_T)0;
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _one)(void) {
    MB_VN r;
    MATHBITS_V_T *p = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        p[i] = (MATHBITS_V_T)1;
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _splat)(MATHBITS_V_T s) {
    MB_VN r;
    MATHBITS_V_T *p = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        p[i] = s;
    }
    return r;
}

#if MATHBITS_V_N == 2
MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _new)(MATHBITS_V_T x, MATHBITS_V_T y) {
    MB_VN r;
    r.x = x;
    r.y = y;
    return r;
}
#elif MATHBITS_V_N == 3
MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _new)(MATHBITS_V_T x, MATHBITS_V_T y, MATHBITS_V_T z) {
    MB_VN r;
    r.x = x;
    r.y = y;
    r.z = z;
    return r;
}
#elif MATHBITS_V_N == 4
MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _new)(MATHBITS_V_T x, MATHBITS_V_T y, MATHBITS_V_T z, MATHBITS_V_T w) {
    MB_VN r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.w = w;
    return r;
}
#endif

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _add)(MB_VN a, MB_VN b) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] + bp[i];
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _sub)(MB_VN a, MB_VN b) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] - bp[i];
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _mul)(MB_VN a, MB_VN b) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] * bp[i];
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _scale)(MB_VN a, MATHBITS_V_T s) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] * s;
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _neg)(MB_VN a) {
    return MATHBITS_PASTE2(MB_VN, _scale)(a, (MATHBITS_V_T)-1);
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _mad)(MB_VN a, MATHBITS_V_T s, MB_VN b) {
    return MATHBITS_PASTE2(MB_VN, _add)(MATHBITS_PASTE2(MB_VN, _scale)(a, s), b);
}

MATHBITS_INLINE void MATHBITS_PASTE2(MB_VN, _add_inplace)(MB_VN *a, MB_VN b) {
    *a = MATHBITS_PASTE2(MB_VN, _add)(*a, b);
}

MATHBITS_INLINE void MATHBITS_PASTE2(MB_VN, _scale_inplace)(MB_VN *a, MATHBITS_V_T s) {
    *a = MATHBITS_PASTE2(MB_VN, _scale)(*a, s);
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _dot)(MB_VN a, MB_VN b) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    MATHBITS_V_T acc = (MATHBITS_V_T)0;
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        acc += ap[i] * bp[i];
    }
    return acc;
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _sum)(MB_VN a) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    MATHBITS_V_T acc = (MATHBITS_V_T)0;
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        acc += ap[i];
    }
    return acc;
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _norm_sq)(MB_VN a) {
    return MATHBITS_PASTE2(MB_VN, _dot)(a, a);
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _norm)(MB_VN a) {
#if defined(MATHBITS_V_T_IS_FLOAT)
    return sqrtf(MATHBITS_PASTE2(MB_VN, _norm_sq)(a));
#else
    return sqrt(MATHBITS_PASTE2(MB_VN, _norm_sq)(a));
#endif
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _norm_inf)(MB_VN a) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    MATHBITS_V_T m = (MATHBITS_V_T)0;
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
#if defined(MATHBITS_V_T_IS_FLOAT)
        MATHBITS_V_T v = fabsf(ap[i]);
#else
        MATHBITS_V_T v = fabs(ap[i]);
#endif
        if (v > m) {
            m = v;
        }
    }
    return m;
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _min_element)(MB_VN a) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    MATHBITS_V_T m = ap[0];
    int i;
    for (i = 1; i < MATHBITS_V_N; ++i) {
        if (ap[i] < m) {
            m = ap[i];
        }
    }
    return m;
}

MATHBITS_INLINE MATHBITS_V_T MATHBITS_PASTE2(MB_VN, _max_element)(MB_VN a) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    MATHBITS_V_T m = ap[0];
    int i;
    for (i = 1; i < MATHBITS_V_N; ++i) {
        if (ap[i] > m) {
            m = ap[i];
        }
    }
    return m;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _min)(MB_VN a, MB_VN b) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] < bp[i] ? ap[i] : bp[i];
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _max)(MB_VN a, MB_VN b) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        rp[i] = ap[i] > bp[i] ? ap[i] : bp[i];
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _clamp)(MB_VN a, MATHBITS_V_T lo, MATHBITS_V_T hi) {
    MB_VN r;
    MATHBITS_V_T *rp = MATHBITS_PASTE2(MB_VN, _ptr)(&r);
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        MATHBITS_V_T v = ap[i];
        rp[i] = v < lo ? lo : (v > hi ? hi : v);
    }
    return r;
}

MATHBITS_INLINE int MATHBITS_PASTE2(MB_VN, _eq)(MB_VN a, MB_VN b, MATHBITS_V_T tol) {
    const MATHBITS_V_T *ap = MATHBITS_PASTE2(MB_VN, _cptr)(&a);
    const MATHBITS_V_T *bp = MATHBITS_PASTE2(MB_VN, _cptr)(&b);
    int i;
    for (i = 0; i < MATHBITS_V_N; ++i) {
        MATHBITS_V_T d = ap[i] - bp[i];
        if (d < (MATHBITS_V_T)0) {
            d = -d;
        }
        if (d > tol) {
            return 0;
        }
    }
    return 1;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _lerp)(MB_VN a, MB_VN b, MATHBITS_V_T t) {
    return MATHBITS_PASTE2(MB_VN, _add)(a, MATHBITS_PASTE2(MB_VN, _scale)(MATHBITS_PASTE2(MB_VN, _sub)(b, a), t));
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _normalize)(MB_VN a) {
    MATHBITS_V_T n = MATHBITS_PASTE2(MB_VN, _norm)(a);
    if (n == (MATHBITS_V_T)0) {
        return MATHBITS_PASTE2(MB_VN, _zero)();
    }
    return MATHBITS_PASTE2(MB_VN, _scale)(a, (MATHBITS_V_T)1 / n);
}

#if MATHBITS_V_N == 3
MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _cross)(MB_VN a, MB_VN b) {
    return MATHBITS_PASTE2(MB_VN, _new)(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _reflect)(MB_VN i, MB_VN n) {
    /* i - 2 dot(n,i) n   (n should be unit) */
    MATHBITS_V_T d = MATHBITS_PASTE2(MB_VN, _dot)(n, i);
    return MATHBITS_PASTE2(MB_VN, _sub)(i, MATHBITS_PASTE2(MB_VN, _scale)(n, d * (MATHBITS_V_T)2));
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_VN, _project)(MB_VN a, MB_VN b) {
    MATHBITS_V_T bb = MATHBITS_PASTE2(MB_VN, _norm_sq)(b);
    if (bb == (MATHBITS_V_T)0) {
        return MATHBITS_PASTE2(MB_VN, _zero)();
    }
    return MATHBITS_PASTE2(MB_VN, _scale)(b, MATHBITS_PASTE2(MB_VN, _dot)(a, b) / bb);
}
#endif

#undef MB_VN
#undef MATHBITS_V_N
#undef MATHBITS_V_T
#undef MATHBITS_V_SFX
#ifdef MATHBITS_V_T_IS_FLOAT
#  undef MATHBITS_V_T_IS_FLOAT
#endif
