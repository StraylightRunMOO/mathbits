/*
 * mathbits - alloc.h
 * Pluggable allocator vtable. Optional memento binding.
 *
 * Default is malloc/free. Define MATHBITS_USE_MEMENTO before including
 * (and include memento.h first) to route through a thread heap.
 *
 * Size is passed on free/realloc so memento's exact-size path works.
 * Views carry their length; *_free helpers compute the byte count.
 *
 * In exactly one translation unit:
 *   #define MATHBITS_IMPLEMENTATION
 *   #include "mathbits/mathbits.h"
 */

#ifndef MATHBITS_ALLOC_H
#define MATHBITS_ALLOC_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*mathbits_alloc_fn)(void *ctx, size_t size);
typedef void *(*mathbits_realloc_fn)(void *ctx, void *ptr, size_t old_size, size_t new_size);
typedef void  (*mathbits_free_fn)(void *ctx, void *ptr, size_t size);

typedef struct mathbits_allocator {
    mathbits_alloc_fn   alloc;
    mathbits_realloc_fn realloc;
    mathbits_free_fn    free_fn;
    void               *ctx;
} mathbits_allocator_t;

MATHBITS_INLINE void *mathbits_default_alloc(void *ctx, size_t size) {
    (void)ctx;
    return malloc(size);
}

MATHBITS_INLINE void *mathbits_default_realloc(void *ctx, void *ptr, size_t old_size, size_t new_size) {
    (void)ctx;
    (void)old_size;
    return realloc(ptr, new_size);
}

MATHBITS_INLINE void mathbits_default_free(void *ctx, void *ptr, size_t size) {
    (void)ctx;
    (void)size;
    free(ptr);
}

MATHBITS_INLINE mathbits_allocator_t mathbits_default_allocator_value(void) {
    mathbits_allocator_t a;
    a.alloc = mathbits_default_alloc;
    a.realloc = mathbits_default_realloc;
    a.free_fn = mathbits_default_free;
    a.ctx = NULL;
    return a;
}

/* Defined in the MATHBITS_IMPLEMENTATION TU. */
void mathbits_set_alloc(const mathbits_allocator_t *a);
mathbits_allocator_t *mathbits_get_alloc(void);

MATHBITS_INLINE void *mathbits_malloc(size_t size) {
    mathbits_allocator_t *a = mathbits_get_alloc();
    return a->alloc(a->ctx, size);
}

MATHBITS_INLINE void *mathbits_realloc(void *ptr, size_t old_size, size_t new_size) {
    mathbits_allocator_t *a = mathbits_get_alloc();
    return a->realloc(a->ctx, ptr, old_size, new_size);
}

MATHBITS_INLINE void mathbits_free(void *ptr, size_t size) {
    if (!ptr) {
        return;
    }
    mathbits_allocator_t *a = mathbits_get_alloc();
    a->free_fn(a->ctx, ptr, size);
}

MATHBITS_INLINE void *mathbits_aligned_alloc(size_t size, size_t alignment) {
    mathbits_allocator_t *a = mathbits_get_alloc();
    if (alignment < sizeof(void *)) {
        alignment = sizeof(void *);
    }
    size_t total = size + alignment + sizeof(void *);
    void *raw = a->alloc(a->ctx, total);
    if (!raw) {
        return NULL;
    }
    uintptr_t aligned = ((uintptr_t)raw + sizeof(void *) + alignment - 1u) & ~(uintptr_t)(alignment - 1u);
    ((void **)aligned)[-1] = raw;
    return (void *)aligned;
}

MATHBITS_INLINE void mathbits_aligned_free(void *ptr, size_t size) {
    if (!ptr) {
        return;
    }
    (void)size;
    mathbits_allocator_t *a = mathbits_get_alloc();
    void *raw = ((void **)ptr)[-1];
    /* total was size + alignment + sizeof(void *); we do not recover exact
     * total, so pass 0 for backends that ignore size (malloc). Memento
     * users should prefer mathbits_malloc of already-aligned arenas. */
    a->free_fn(a->ctx, raw, 0);
}

#if defined(MATHBITS_USE_MEMENTO)
#ifndef MEMENTO_H
#  error "MATHBITS_USE_MEMENTO requires #include \"memento.h\" before mathbits alloc.h"
#endif
/* Caller included memento.h and (in the implementation TU) MEMENTO_IMPLEMENTATION. */
MATHBITS_INLINE void *mathbits_memento_alloc(void *ctx, size_t size) {
    memento_thread_heap_t *heap = (memento_thread_heap_t *)ctx;
    if (!heap) {
        heap = memento_thread_heap_get();
    }
    return memento_thread_heap_alloc(heap, size);
}

MATHBITS_INLINE void *mathbits_memento_realloc(void *ctx, void *ptr, size_t old_size, size_t new_size) {
    memento_thread_heap_t *heap = (memento_thread_heap_t *)ctx;
    if (!heap) {
        heap = memento_thread_heap_get();
    }
    return memento_thread_heap_realloc(heap, ptr, old_size, new_size);
}

MATHBITS_INLINE void mathbits_memento_free(void *ctx, void *ptr, size_t size) {
    memento_thread_heap_t *heap = (memento_thread_heap_t *)ctx;
    if (!heap) {
        heap = memento_thread_heap_get();
    }
    memento_thread_heap_free(heap, ptr, size);
}

MATHBITS_INLINE mathbits_allocator_t mathbits_memento_allocator(memento_thread_heap_t *heap) {
    mathbits_allocator_t a;
    a.alloc = mathbits_memento_alloc;
    a.realloc = mathbits_memento_realloc;
    a.free_fn = mathbits_memento_free;
    a.ctx = heap;
    return a;
}
#endif /* MATHBITS_USE_MEMENTO */

#ifdef MATHBITS_IMPLEMENTATION
static mathbits_allocator_t mathbits_g_alloc;
static int mathbits_g_alloc_set;

void mathbits_set_alloc(const mathbits_allocator_t *a) {
    if (a) {
        mathbits_g_alloc = *a;
        mathbits_g_alloc_set = 1;
    } else {
        mathbits_g_alloc_set = 0;
    }
}

mathbits_allocator_t *mathbits_get_alloc(void) {
    if (!mathbits_g_alloc_set) {
        mathbits_g_alloc = mathbits_default_allocator_value();
        mathbits_g_alloc_set = 1;
    }
    return &mathbits_g_alloc;
}
#endif /* MATHBITS_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* MATHBITS_ALLOC_H */
