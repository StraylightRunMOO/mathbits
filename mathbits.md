# PLAN.md — mathbits

A header-only, pure C11 numerical library for vectors, matrices, tensors, small neural nets, and related primitives. Designed so consumers pull in only what they need. Heavy preprocessor use (in the style of [swizz.h](https://github.com/StraylightRunMOO/swizz.h)) generates type-safe, fixed-size specialized code with Eigen-like ergonomics and performance while remaining idiomatic C11.

The library lives in the StraylightRunMOO / NotoriousLOB / kevinfling ecosystem and is intended to become the shared dense linear-algebra substrate for other projects. It is **not** a replacement for specialized libraries; it sits underneath or beside them.

## Related repositories (context and intended relationships)

- [memento](https://github.com/StraylightRunMOO/memento) — high-performance allocator (arenas, thread-local heaps). mathbits uses it optionally for all dynamic allocation and scratch space.
- [suspenders.h](https://github.com/StraylightRunMOO/suspenders.h) — stackful coroutines, scheduler, channels. Used only in examples for concurrent batch work; core mathbits stays single-threaded and allocation-explicit.
- [swizz.h](https://github.com/StraylightRunMOO/swizz.h) — preprocessor-based Swiss Table generator. The model for how mathbits uses macros to produce type-safe, zero-overhead specializations.
- [SIGMA](https://github.com/StraylightRunMOO/SIGMA) — statistical distributions, PRNG, moments. mathbits does not reimplement these; it may offer thin helpers that delegate to SIGMA.
- [noize.h](https://github.com/StraylightRunMOO/noize.h) — spectral/spatial noise, QMC, filters. Remains separate; mathbits supplies vector/matrix views it can consume.
- [curves.h](https://github.com/StraylightRunMOO/curves.h) — space-filling curves and discrete grid indexing. Complementary; no overlap with dense linear algebra.
- [chroma](https://github.com/StraylightRunMOO/chroma) / [ChromaCrusher](https://github.com/StraylightRunMOO/ChromaCrusher) — color-space conversion and quantization. Domain-specific kernels stay in chroma; mathbits can supply generic small-matrix primitives if useful later.
- [Notorious-FFT](https://github.com/NotoriousLOB/Notorious-FFT) / [FastAndFourier](https://github.com/NotoriousLOB/FastAndFourier) — FFT and DSP. mathbits provides views and higher-level helpers; the transforms themselves stay in those libraries.
- [tensio](https://github.com/NotoriousLOB/tensio) — Safetensors/GGUF tensor I/O. mathbits tensors can be views over tensio-loaded data.
- [lagrange](https://github.com/kevinfling/lagrange) — header-only C11 rigid-body and orbital physics. Vector, quaternion, and generic integrator pieces are candidates for extraction into mathbits; lagrange then depends on mathbits.
- [reaper](https://github.com/StraylightRunMOO/reaper), [halyard](https://github.com/StraylightRunMOO/halyard), [telnetty.h](https://github.com/StraylightRunMOO/telnetty.h) — other header-only C11 runtime pieces that illustrate the same inclusion and style conventions.

## Design principles

- Header-only C11. Classic `#define MATHBITS_IMPLEMENTATION` (or per-module equivalents) in exactly one translation unit.
- Preprocessor as the template engine (swizz.h style). Macros generate fixed-size types, unrolled kernels, and namespaced functions. Dynamic sizes use optimized general paths.
- Zero-overhead views by default. Owning containers are explicit and allocator-driven.
- Performance first for the sizes that matter (fixed small vectors/matrices and medium dense work inside small MLPs, geometry, and physics steps). SIMD (AVX2/AVX-512/NEON) at compile time with scalar fallbacks. Restrict-qualified pointers, alignment hints, no hidden temporaries on hot paths.
- Optional memento integration. When enabled, all allocations and scratch come from caller-supplied memento heaps/arenas. Fallback to malloc/free or user hooks otherwise.
- Optional external BLAS/LAPACK. Default path is self-contained and high-quality for small/medium sizes. A compile-time switch can redirect large gemm/gemv/etc. to CBLAS. Not required for v0.1.
- Hierarchical includes so consumers minimize footprint.
- Pure C11 core. A later thin C++ facade will target near drop-in compatibility with common Eigen usage (fixed-size Matrix/Vector, Map, basic arithmetic, blocks, small decompositions). The C kernels remain the source of truth; the C++ layer is zero-overhead wrapping.
- No hidden globals, no exceptions, no RTTI in the C core.

## Hierarchical layout

```
include/mathbits/
├── mathbits.h              # optional umbrella
├── config.h                # precision, alignment, SIMD, allocator switches
├── alloc.h                 # memento-optional allocator vtable + fallback
├── num.h                   # scalar numeric helpers, constants, safe arithmetic
│
├── vec.h                   # vectors (fixed via macros + dynamic views)
├── mat.h                   # matrices; depends on vec.h
├── tensor.h                # rank ≥ 3; depends on mat.h / vec.h
│
├── mat/                    # specialized matrix kinds (opt-in)
│   ├── adjacency.h
│   ├── transition.h
│   └── sparse.h
│
├── linalg.h                # higher-level dense ops, small solvers, decompositions
├── nn.h                    # dense layers + sequential inference
├── geometry.h              # quaternions, rigid transforms, bounding volumes
├── optim.h                 # GD, L-BFGS, Levenberg-Marquardt, small NLS
│
└── samplers/               # purpose-built utilities
    ├── gibbs.h
    └── metropolis.h
```

Dependency direction is strictly upward: `num` / `vec` ← `mat` ← `tensor` ← specialized headers and higher-level modules. A consumer can include only `vec.h` (plus config/alloc if needed) and get a complete, fast vector API.

## Core modules for v0.1

### num.h
Scalar helpers: constants, safe comparisons, basic special functions that are not already in SIGMA, type traits via macros, and numeric utilities shared by vec/mat. Keeps higher headers free of scattered scalar logic.

### vec.h
Fixed-size vectors generated by macros (`vec2f`, `vec3d`, `vec4f`, …) plus dynamic views. Operations: arithmetic, norms, reductions, dot, cross (for 3D), normalization, swizzling via macros. SIMD specializations for common widths. Primary abstraction is a non-owning view; owning storage is explicit.

### mat.h
Row-major (column-major optional) matrices as views over contiguous or strided data. Builds on vec.h. Core operations: element-wise, gemv, gemm (excellent small/medium performance), outer products, rank-1 updates, transpose views, block views. Tiny fixed-size solvers (2×2, 3×3, 4×4) via explicit formulas. Macro-generated fixed-size matrix types.

### tensor.h
Rank-3+ views built on mat/vec. Basic slicing, reductions, and contraction helpers sufficient for small NN activations and geometry. Not a full tensor engine.

### linalg.h
Higher-level dense linear algebra that does not belong in the lowest headers: Cholesky, QR, SVD, and least-squares for small-to-medium sizes (N ≲ 64–128, macro-specialized for the tiniest). Optional path to external LAPACK for larger problems.

### nn.h
dlib-style practical dense inference. Layer descriptors (Dense, Activation: ReLU, GELU, sigmoid, tanh, softmax). Sequential model runner. Weight loading from C arrays or flat binary blobs. Batch support via mat.h gemm. Designed for the landmark regressors and category classifiers; activations and intermediates live in caller- or memento-owned memory.

### geometry.h
Quaternions, rigid transforms, simple bounding volumes. Extracted and generalized from lagrange’s math/transform pieces so lagrange can later depend on mathbits.

### optim.h
Small-scale optimization: gradient descent, L-BFGS, Levenberg-Marquardt. Useful for calibrating small networks or sensor models. Operates on vec/mat views.

### Specialized and utility headers
- `mat/adjacency.h`, `mat/transition.h`, `mat/sparse.h`: thin opt-in extensions that add constructors, invariants, or specialized kernels while reusing mat.h storage and arithmetic.
- `samplers/gibbs.h`, `samplers/metropolis.h`: example purpose-built utilities that sit on top of the core. Demonstrate the extension model; not required for the marina/vision use case but show the intended growth path.

## Memory, SIMD, and performance model

- All dynamic memory goes through the allocator vtable in `alloc.h`. Define the appropriate macro before inclusion to bind it to memento; otherwise a simple malloc fallback is used.
- Scratch space for blocked algorithms is caller-supplied or drawn from a memento arena.
- SIMD kernels (AVX2/NEON first, AVX-512 where clean) are selected at compile time. Scalar versions always exist.
- For fixed sizes that appear in NN heads, geometry, and physics, macro unrolling + explicit SIMD is expected to match or exceed typical Eigen performance because there is no expression-template or abstraction overhead and the specializations are exact.
- Large dense operations may optionally call CBLAS; this is off by default and not required for v0.1.

## C++ facade (post-v0.1 but designed for)

A thin C++ layer (separate headers or a single compat header) providing templates, RAII where ownership is desired, and an API as close as practical to common Eigen usage. Fixed-size types wrap the macro-generated C objects. Hot paths call the C kernels directly. Full Eigen compatibility is not the goal; “most existing Eigen-using code for small-to-medium dense work compiles and runs with minimal changes” is.

## v0.1 implementation order (one-shot)

Implement in this order so each step is usable and testable:

1. Repository skeleton, license (CMIT/MIT), `config.h`, `alloc.h` (memento-optional + fallback), basic test/benchmark harness, and README with include examples.
2. `num.h` — scalar constants, safe arithmetic, shared numeric helpers.
3. `vec.h` — macro-generated fixed-size vectors, dynamic views, arithmetic, norms, dot/cross, SIMD paths, tests against reference values.
4. `mat.h` — views, element-wise, gemv, gemm (small/medium optimized), outer products, tiny fixed solvers (2×2/3×3/4×4), tests and benchmarks.
5. `linalg.h` — Cholesky, QR, least-squares for small/medium sizes; optional external LAPACK switch (can be stubbed).
6. `tensor.h` — basic rank-3+ views, slicing, and reductions sufficient for NN activations.
7. `nn.h` — Dense + Activation layers, sequential forward, weight loading (C array + flat blob), batch support, example landmark-style MLP.
8. `geometry.h` — quaternions and rigid transforms (extracted/generalized from lagrange style), tests.
9. `optim.h` — gradient descent and L-BFGS (or LM) operating on vec/mat views, simple tests.
10. Specialized matrix headers (`mat/adjacency.h`, `mat/transition.h`) as thin demonstrations.
11. Example programs: basic vector/matrix use, MLP inference, memento arena usage, and one suspenders.h batch example.
12. Documentation pass (API comments in the style of memento/swizz) and final benchmark comparison on the target small/medium sizes.

At the end of this list the library must be able to:
- Express and run the category classifier + landmark regressor entirely on mathbits.
- Serve as the vector/matrix foundation that lagrange’s generic math can later migrate onto.
- Allow a consumer to include only the headers they need.
- Demonstrate optional memento allocation and a clear path for the future C++ Eigen-flavored facade.

All code remains pure C11, header-only, and consistent with the preprocessor and inclusion patterns used in swizz.h, memento, and suspenders.h.
