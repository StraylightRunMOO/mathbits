# mathbits

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Standard](https://img.shields.io/badge/C-11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))

Header-only C11 dense linear algebra: fixed-size vectors and matrices generated like [swizz.h](https://github.com/StraylightRunMOO/swizz.h), dynamic views, small-to-medium GEMM, factorizations, a practical dense MLP, and geometry types that match [lagrange](https://github.com/kevinfling/lagrange).

It is the shared numeric substrate for the StraylightRunMOO / NotoriousLOB stack. It is not a BLAS, not SIGMA, and not Eigen.

## Quick start

```c
#define MATHBITS_IMPLEMENTATION   /* exactly one TU */
#include "mathbits/mathbits.h"

int main(void) {
    vec3f a = vec3f_new(1.0f, 2.0f, 3.0f);
    vec3f n = vec3f_normalize(a);
    mat3f R = mat3f_identity();
    vec3f b = mat3f_mul_vec(R, n);
    (void)b;
    return 0;
}
```

Include only what you need: `"mathbits/vec.h"` is a complete vector API.

## Layout

```
include/mathbits/
  mathbits.h     umbrella
  config.h       SIMD, alignment, attributes
  alloc.h        vtable; optional memento
  num.h          scalars
  vec.h          fixed + views
  mat.h          views, gemv, gemm
  linalg.h       Cholesky, QR, SVD, lstsq
  tensor.h       rank ≥ 3 views
  nn.h           dense sequential inference
  geometry.h     quat, rigid, AABB, integrators
  optim.h        GD, L-BFGS
  mat/           sparse, adjacency, transition
  samplers/      Gibbs, Metropolis-Hastings
```

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/examples/ex_mlp
./build/benchmarks/mathbits_bench
```

| Option | Default | Role |
|---|---|---|
| `MATHBITS_BUILD_TESTS` | ON | unit tests |
| `MATHBITS_BUILD_BENCHMARKS` | ON | gemm / vec3 benches |
| `MATHBITS_BUILD_EXAMPLES` | ON | `ex_basic`, `ex_mlp` |
| `MATHBITS_ENABLE_NATIVE` | ON | `-march=native` on tests/benches only |
| `MATHBITS_SANITIZE` | off | `address,undefined` or `thread` |
| `MATHBITS_USE_MEMENTO` | OFF | FetchContent memento v3.0.0 |
| `MATHBITS_MEMENTO_SOURCE` | | local memento tree |

The public `mathbits` INTERFACE target does not leak warning or native flags. Those sit on `mathbits_internal`.

```cmake
add_subdirectory(mathbits)
target_link_libraries(app PRIVATE mathbits::mathbits)
```

## Memory and SIMD

Views (`vec_view`, `mat_view`) never allocate. `*_alloc` uses the current allocator (malloc, or memento when enabled). Multi-step algorithms write into caller storage plus optional scratch.

```c
#define MATHBITS_USE_MEMENTO
#include "memento.h"
#include "mathbits/alloc.h"

memento_init();
mathbits_allocator_t a = mathbits_memento_allocator(memento_thread_heap_get());
mathbits_set_alloc(&a);
```

SIMD is compile-time: AVX2+FMA, AVX-512, or NEON, with a scalar path always present. Tiny 2×2 / 3×3 / 4×4 kernels are fully unrolled.

On this aarch64 host (clang 22, `-mcpu=native`), `mathbits_bench` reported about 10 GFLOP/s on 64×64 and 128×128 `sgemm`.

## Siblings

| Library | Relationship |
|---|---|
| [memento](https://github.com/StraylightRunMOO/memento) | optional allocator |
| [suspenders.h](https://github.com/StraylightRunMOO/suspenders.h) | examples only; core is single-threaded |
| [SIGMA](https://github.com/StraylightRunMOO/SIGMA) | distributions / PRNG stay there |
| [lagrange](https://github.com/kevinfling/lagrange) | `vec3f` / `quat_f` layout-compatible |
| tensio, Notorious-FFT, noize | consume `vec_view` / `tensor_view` |

A later C++ facade will wrap these kernels for common Eigen usage. The C API is the source of truth.

## License

MIT — see [LICENSE](LICENSE). Copyright (c) 2026 Straylight Run MOO.
