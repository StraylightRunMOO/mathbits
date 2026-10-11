/*
 * mathbits - header-only C11 dense linear algebra
 * https://github.com/StraylightRunMOO/mathbits
 *
 * config.h - version, platform, SIMD, compiler attributes
 *
 * License: MIT
 */

#ifndef MATHBITS_CONFIG_H
#define MATHBITS_CONFIG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MATHBITS_VERSION_MAJOR 0
#define MATHBITS_VERSION_MINOR 1
#define MATHBITS_VERSION_PATCH 0
#define MATHBITS_VERSION_STRING "0.1.0"

/* ========================================================================
 * Architecture / SIMD
 * ======================================================================== */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#  define MATHBITS_ARCH_X86 1
#else
#  define MATHBITS_ARCH_X86 0
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#  define MATHBITS_ARCH_ARM 1
#else
#  define MATHBITS_ARCH_ARM 0
#endif

#if MATHBITS_ARCH_X86 && defined(__AVX512F__)
#  define MATHBITS_HAS_AVX512 1
#  include <immintrin.h>
#else
#  define MATHBITS_HAS_AVX512 0
#endif

#if MATHBITS_ARCH_X86 && defined(__AVX2__)
#  define MATHBITS_HAS_AVX2 1
#  include <immintrin.h>
#else
#  define MATHBITS_HAS_AVX2 0
#endif

#if MATHBITS_ARCH_ARM && (defined(__ARM_NEON) || defined(__ARM_NEON__))
#  define MATHBITS_HAS_NEON 1
#  include <arm_neon.h>
#else
#  define MATHBITS_HAS_NEON 0
#endif

#if MATHBITS_HAS_AVX512
#  define MATHBITS_SIMD_WIDTH_F 16
#  define MATHBITS_SIMD_WIDTH_D 8
#elif MATHBITS_HAS_AVX2
#  define MATHBITS_SIMD_WIDTH_F 8
#  define MATHBITS_SIMD_WIDTH_D 4
#elif MATHBITS_HAS_NEON
#  define MATHBITS_SIMD_WIDTH_F 4
#  if defined(__aarch64__)
#    define MATHBITS_SIMD_WIDTH_D 2
#  else
#    define MATHBITS_SIMD_WIDTH_D 0
#  endif
#else
#  define MATHBITS_SIMD_WIDTH_F 0
#  define MATHBITS_SIMD_WIDTH_D 0
#endif

/* ========================================================================
 * Compiler attributes
 * ======================================================================== */
#if defined(__GNUC__) || defined(__clang__)
#  define MATHBITS_INLINE   static inline __attribute__((always_inline))
#  define MATHBITS_NOINLINE __attribute__((noinline))
#  define MATHBITS_LIKELY(x)   __builtin_expect(!!(x), 1)
#  define MATHBITS_UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define MATHBITS_RESTRICT __restrict
#  define MATHBITS_ALIGNED(x) __attribute__((aligned(x)))
#  define MATHBITS_PREFETCH(addr, rw, loc) __builtin_prefetch((addr), (rw), (loc))
#  define MATHBITS_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#  define MATHBITS_INLINE   static __forceinline
#  define MATHBITS_NOINLINE __declspec(noinline)
#  define MATHBITS_LIKELY(x)   (x)
#  define MATHBITS_UNLIKELY(x) (x)
#  define MATHBITS_RESTRICT __restrict
#  define MATHBITS_ALIGNED(x) __declspec(align(x))
#  define MATHBITS_PREFETCH(addr, rw, loc) _mm_prefetch((const char *)(addr), (loc))
#  define MATHBITS_UNUSED
#else
#  define MATHBITS_INLINE   static inline
#  define MATHBITS_NOINLINE
#  define MATHBITS_LIKELY(x)   (x)
#  define MATHBITS_UNLIKELY(x) (x)
#  define MATHBITS_RESTRICT
#  define MATHBITS_ALIGNED(x)
#  define MATHBITS_PREFETCH(addr, rw, loc) ((void)0)
#  define MATHBITS_UNUSED
#endif

#ifndef MATHBITS_ALIGN
#  define MATHBITS_ALIGN 32
#endif

#define MATHBITS_CACHE_LINE 64
#define MATHBITS_ALIGN_CACHE MATHBITS_ALIGNED(MATHBITS_CACHE_LINE)

#ifndef MATHBITS_ROW_MAJOR
#  define MATHBITS_ROW_MAJOR 1
#endif

/* Token paste helpers for the type generators */
#define MATHBITS_PASTE2_(a, b) a##b
#define MATHBITS_PASTE2(a, b) MATHBITS_PASTE2_(a, b)
#define MATHBITS_PASTE3_(a, b, c) a##b##c
#define MATHBITS_PASTE3(a, b, c) MATHBITS_PASTE3_(a, b, c)
#define MATHBITS_PASTE4_(a, b, c, d) a##b##c##d
#define MATHBITS_PASTE4(a, b, c, d) MATHBITS_PASTE4_(a, b, c, d)
#define MATHBITS_PASTE5_(a, b, c, d, e) a##b##c##d##e
#define MATHBITS_PASTE5(a, b, c, d, e) MATHBITS_PASTE5_(a, b, c, d, e)

#define MATHBITS_STRINGIFY_(x) #x
#define MATHBITS_STRINGIFY(x) MATHBITS_STRINGIFY_(x)

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_CONFIG_H */
