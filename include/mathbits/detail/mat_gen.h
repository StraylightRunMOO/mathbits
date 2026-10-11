#ifndef MATHBITS_M_R
#  error "MATHBITS_M_R/C/T/SFX required"
#endif

#if MATHBITS_M_R == MATHBITS_M_C
#  define MB_MN MATHBITS_PASTE3(mat, MATHBITS_M_R, MATHBITS_M_SFX)
#else
#  define MB_MN MATHBITS_PASTE5(mat, MATHBITS_M_R, x, MATHBITS_M_C, MATHBITS_M_SFX)
#endif

#define MB_VN MATHBITS_PASTE3(vec, MATHBITS_M_C, MATHBITS_M_SFX)
#define MB_VR MATHBITS_PASTE3(vec, MATHBITS_M_R, MATHBITS_M_SFX)

typedef struct MB_MN {
    MATHBITS_M_T m[MATHBITS_M_R][MATHBITS_M_C];
} MB_MN;

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _zero)(void) {
    MB_MN a;
    memset(&a, 0, sizeof(a));
    return a;
}

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _identity)(void) {
    MB_MN a = MATHBITS_PASTE2(MB_MN, _zero)();
    int i, n = MATHBITS_M_R < MATHBITS_M_C ? MATHBITS_M_R : MATHBITS_M_C;
    for (i = 0; i < n; ++i) {
        a.m[i][i] = (MATHBITS_M_T)1;
    }
    return a;
}

MATHBITS_INLINE MATHBITS_M_T MATHBITS_PASTE2(MB_MN, _get)(MB_MN a, int r, int c) {
    return a.m[r][c];
}

MATHBITS_INLINE void MATHBITS_PASTE2(MB_MN, _set)(MB_MN *a, int r, int c, MATHBITS_M_T v) {
    a->m[r][c] = v;
}

#if MATHBITS_M_R == MATHBITS_M_C && MATHBITS_M_R == 2
MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _new)(
    MATHBITS_M_T a00, MATHBITS_M_T a01,
    MATHBITS_M_T a10, MATHBITS_M_T a11) {
    MB_MN a;
    a.m[0][0] = a00; a.m[0][1] = a01;
    a.m[1][0] = a10; a.m[1][1] = a11;
    return a;
}
#elif MATHBITS_M_R == MATHBITS_M_C && MATHBITS_M_R == 3
MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _new)(
    MATHBITS_M_T a00, MATHBITS_M_T a01, MATHBITS_M_T a02,
    MATHBITS_M_T a10, MATHBITS_M_T a11, MATHBITS_M_T a12,
    MATHBITS_M_T a20, MATHBITS_M_T a21, MATHBITS_M_T a22) {
    MB_MN a;
    a.m[0][0] = a00; a.m[0][1] = a01; a.m[0][2] = a02;
    a.m[1][0] = a10; a.m[1][1] = a11; a.m[1][2] = a12;
    a.m[2][0] = a20; a.m[2][1] = a21; a.m[2][2] = a22;
    return a;
}
#endif

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _add)(MB_MN a, MB_MN b) {
    MB_MN r;
    int i, j;
    for (i = 0; i < MATHBITS_M_R; ++i) {
        for (j = 0; j < MATHBITS_M_C; ++j) {
            r.m[i][j] = a.m[i][j] + b.m[i][j];
        }
    }
    return r;
}

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _muls)(MB_MN a, MATHBITS_M_T s) {
    MB_MN r;
    int i, j;
    for (i = 0; i < MATHBITS_M_R; ++i) {
        for (j = 0; j < MATHBITS_M_C; ++j) {
            r.m[i][j] = a.m[i][j] * s;
        }
    }
    return r;
}

#if MATHBITS_M_R == MATHBITS_M_C
MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _mul)(MB_MN a, MB_MN b) {
    MB_MN r = MATHBITS_PASTE2(MB_MN, _zero)();
    int i, j, k;
    for (i = 0; i < MATHBITS_M_R; ++i) {
        for (k = 0; k < MATHBITS_M_C; ++k) {
            MATHBITS_M_T aik = a.m[i][k];
            for (j = 0; j < MATHBITS_M_C; ++j) {
                r.m[i][j] += aik * b.m[k][j];
            }
        }
    }
    return r;
}

MATHBITS_INLINE MB_VN MATHBITS_PASTE2(MB_MN, _mul_vec)(MB_MN a, MB_VN x) {
    MB_VN y = MATHBITS_PASTE2(MB_VN, _zero)();
    MATHBITS_M_T *yp = MATHBITS_PASTE2(MB_VN, _ptr)(&y);
    const MATHBITS_M_T *xp = MATHBITS_PASTE2(MB_VN, _cptr)(&x);
    int i, j;
    for (i = 0; i < MATHBITS_M_R; ++i) {
        MATHBITS_M_T s = 0;
        for (j = 0; j < MATHBITS_M_C; ++j) {
            s += a.m[i][j] * xp[j];
        }
        yp[i] = s;
    }
    return y;
}

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _transpose)(MB_MN a) {
    MB_MN r;
    int i, j;
    for (i = 0; i < MATHBITS_M_R; ++i) {
        for (j = 0; j < MATHBITS_M_C; ++j) {
            r.m[j][i] = a.m[i][j];
        }
    }
    return r;
}
#endif

#if MATHBITS_M_R == 2 && MATHBITS_M_C == 2
MATHBITS_INLINE MATHBITS_M_T MATHBITS_PASTE2(MB_MN, _determinant)(MB_MN a) {
    return a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
}

MATHBITS_INLINE int MATHBITS_PASTE2(MB_MN, _inverse)(const MB_MN *a, MB_MN *out) {
    MATHBITS_M_T det = MATHBITS_PASTE2(MB_MN, _determinant)(*a);
    if (det == (MATHBITS_M_T)0) {
        return 0;
    }
    MATHBITS_M_T inv = (MATHBITS_M_T)1 / det;
    out->m[0][0] =  a->m[1][1] * inv;
    out->m[0][1] = -a->m[0][1] * inv;
    out->m[1][0] = -a->m[1][0] * inv;
    out->m[1][1] =  a->m[0][0] * inv;
    return 1;
}
#endif

#if MATHBITS_M_R == 3 && MATHBITS_M_C == 3
MATHBITS_INLINE MATHBITS_M_T MATHBITS_PASTE2(MB_MN, _determinant)(MB_MN a) {
    return a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])
         - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])
         + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
}

MATHBITS_INLINE int MATHBITS_PASTE2(MB_MN, _inverse)(const MB_MN *a, MB_MN *out) {
    MATHBITS_M_T det = MATHBITS_PASTE2(MB_MN, _determinant)(*a);
    if (det == (MATHBITS_M_T)0) {
        return 0;
    }
    MATHBITS_M_T inv = (MATHBITS_M_T)1 / det;
    out->m[0][0] = (a->m[1][1] * a->m[2][2] - a->m[1][2] * a->m[2][1]) * inv;
    out->m[0][1] = (a->m[0][2] * a->m[2][1] - a->m[0][1] * a->m[2][2]) * inv;
    out->m[0][2] = (a->m[0][1] * a->m[1][2] - a->m[0][2] * a->m[1][1]) * inv;
    out->m[1][0] = (a->m[1][2] * a->m[2][0] - a->m[1][0] * a->m[2][2]) * inv;
    out->m[1][1] = (a->m[0][0] * a->m[2][2] - a->m[0][2] * a->m[2][0]) * inv;
    out->m[1][2] = (a->m[0][2] * a->m[1][0] - a->m[0][0] * a->m[1][2]) * inv;
    out->m[2][0] = (a->m[1][0] * a->m[2][1] - a->m[1][1] * a->m[2][0]) * inv;
    out->m[2][1] = (a->m[0][1] * a->m[2][0] - a->m[0][0] * a->m[2][1]) * inv;
    out->m[2][2] = (a->m[0][0] * a->m[1][1] - a->m[0][1] * a->m[1][0]) * inv;
    return 1;
}

MATHBITS_INLINE MB_MN MATHBITS_PASTE2(MB_MN, _scale_vec)(MB_VN s) {
    MB_MN a = MATHBITS_PASTE2(MB_MN, _zero)();
    a.m[0][0] = s.x;
    a.m[1][1] = s.y;
    a.m[2][2] = s.z;
    return a;
}
#endif

#if MATHBITS_M_R == 4 && MATHBITS_M_C == 4
MATHBITS_INLINE int MATHBITS_PASTE2(MB_MN, _inverse)(const MB_MN *a, MB_MN *out) {
    /* Gauss-Jordan on a copy. */
    MATHBITS_M_T m[4][8];
    int i, j, k;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            m[i][j] = a->m[i][j];
            m[i][j + 4] = (i == j) ? (MATHBITS_M_T)1 : (MATHBITS_M_T)0;
        }
    }
    for (i = 0; i < 4; ++i) {
        int piv = i;
        MATHBITS_M_T best = m[i][i] < 0 ? -m[i][i] : m[i][i];
        for (k = i + 1; k < 4; ++k) {
            MATHBITS_M_T v = m[k][i] < 0 ? -m[k][i] : m[k][i];
            if (v > best) {
                best = v;
                piv = k;
            }
        }
        if (best == (MATHBITS_M_T)0) {
            return 0;
        }
        if (piv != i) {
            for (j = 0; j < 8; ++j) {
                MATHBITS_M_T t = m[i][j];
                m[i][j] = m[piv][j];
                m[piv][j] = t;
            }
        }
        {
            MATHBITS_M_T diag = m[i][i];
            for (j = 0; j < 8; ++j) {
                m[i][j] /= diag;
            }
        }
        for (k = 0; k < 4; ++k) {
            if (k == i) {
                continue;
            }
            MATHBITS_M_T f = m[k][i];
            for (j = 0; j < 8; ++j) {
                m[k][j] -= f * m[i][j];
            }
        }
    }
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            out->m[i][j] = m[i][j + 4];
        }
    }
    return 1;
}
#endif

#undef MB_MN
#undef MB_VN
#undef MB_VR
#undef MATHBITS_M_R
#undef MATHBITS_M_C
#undef MATHBITS_M_T
#undef MATHBITS_M_SFX
