#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <stddef.h>

#define MAX_ALLOC_SIZE    16711568
#define MAX_ARRAYS_COUNT  (1ul << 8)
#define MAX_VECTOR_SIZE   -1u
#define MAX_VECTORS_COUNT (1ul << 20)
#define MAX_STACK_HEIGHT  (1ul << 20)

#define __vt_min(a, b) ((a)<(b) ? a : b)
#define __vt_max(a, b) ((a)>(b) ? a : b)

#define vt_begin                  __vt_scope_begin
#define vt_end                    __vt_scope_end
#define vt_new(count, type)       (__vt_new(count, sizeof(type)))
// #define vt_delete(vt)             (__vt_delete(vt)) // deprecates.
#define vt_count(vt)              (__vt_count(vt))
#define vt_size(vt)               vt_count(vt)
#define vt_length(vt)             vt_count(vt)
#define vt_type_size(vt)          (__vt_type_size(vt))
#define vt_type_count(vt)         (__vt_type_count(vt))
#define vt_capacity(vt)           (__vt_capacity(vt))
#define vt_empty(vt)              (__vt_empty(vt))
#define vt_resize(vt, count)      (__vt_resize(vt, count))
#define vt_clear(vt)              vt_resize(vt, 0)
#define vt_add(vt, item)          (__vt_add(vt, &item)) // only variable.
#define vt_push(vt, item)         (__vt_push(vt, item)) // only literal/ptr.
#define vt_pop(vt)                (__vt_pop(vt))
#define vt_at(vt, idx, type)      (*(type *)__vt_at(vt, idx))
#define vt_front(vt, type)        vt_at(vt, 0, type)
#define vt_back(vt, type)         vt_at(vt, vt_count(vt) - 1, type)
#define vt_move(dst, src)         (__vt_move(dst, src))
#define vt_copy(dst, src)         (__vt_copy(dst, src)) // vt_copy: not support
                                                        // multidimensional
                                                        // vector.

typedef struct {
  void   *arrays[MAX_ARRAYS_COUNT];
  size_t  arrays_counts[MAX_ARRAYS_COUNT];
  size_t  arrays_count;
  size_t  type_size;
  size_t  count;
  size_t  __max_alloc_count;
} *vector, _vector;

void    __vt_scope_begin(void);
void    __vt_scope_end(void);
vector  __vt_new(size_t count, size_t type_size);
void    __vt_delete(vector self);
size_t  __vt_type_size(vector self);
size_t  __vt_type_count(vector self);
size_t  __vt_capacity(vector self);
size_t  __vt_count(vector self);
int     __vt_empty(vector self);
int     __vt_resize(vector self, size_t count);
int     __vt_add(vector self, void *item);
int     __vt_push(vector self, ...);
void    __vt_pop(vector self);
void   *__vt_at(vector self, size_t idx);
void    __vt_move(vector dst, vector src);
void    __vt_copy(vector dst, vector src);

#endif // VECTOR_VECTOR_H
