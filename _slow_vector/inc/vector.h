#ifndef __VECTOR_VECTOR_H
#define __VECTOR_VECTOR_H

#include <stddef.h>

#define __MAX_VECTORS_COUNT     (1ull << 20)
#define __MAX_STACK_HEIGHT      (1ull << 20)
#define __MAX_ARRAYS_COUNT      (1ull << 5)
#define __MAX_VECTOR_DIMENSION  (1ull << 5)

#define __nullptr   (void *)0
#define __true      1
#define __false     0

//
//  When using multithreading with vectors.
//  __VECTOR_MULTITHREAD must be defined.
//
// #define __VECTOR_MULTITHREAD
#ifndef __VECTOR_MULTITHREAD
# define vt_in                    __vt_stack_in   // single-threading.
# define vt_out                   __vt_stack_out  // single-threading.
#else
# define vt_delete(vt)            (__vt_delete(vt, __true)) // multi-threading.
#endif
#define vt_new(type, di, ...)     (__vt_new(sizeof(type), di, __VA_ARGS__))
#define vt_count(vt)              (__vt_count(vt))
#define vt_size(vt)               vt_count(vt)
#define vt_len(vt)                vt_count(vt)
#define vt_empty(vt)              (vt_count(vt) == 0)
#define vt_type_size(vt)          (__vt_type_size(vt))
#define vt_type_count(vt)         (__vt_type_count(vt))
#define vt_capacity(vt)           (__vt_capacity(vt))
#define vt_resize(vt, count)      (__vt_resize(vt, count))
#define vt_clear(vt)              (__vt_clear(vt))
#define vt_add(vt, item)          (__vt_add(vt, &item))   // variable.
#define vt_push(vt, item)         (__vt_push(vt, item))   // literal.
#define vt_pop(vt)                (__vt_pop(vt))
#define vt_at(vt, idx, type)      (*(type *)__vt_at(vt, idx))
#define vt_front(vt, type)        (*(type *)__vt_front(vt))
#define vt_back(vt, type)         (*(type *)__vt_back(vt))
#define vt_move(src)              (__vt_move(&src))
#define vt_clone(src)             (__vt_clone(src))
#define vt_swap(dst, src)         (__vt_swap(&dst, &src))
#define vt_reverse(vt)            (__vt_reverse(vt))
#define vt_itr(vt, idx)           (__vt_iterator(vt, idx))
#define vt_begin(vt)              (__vt_begin(vt))
#define vt_end(vt)                (__vt_end(vt))
#define vt_next(itr)              (__vt_itr_next(&itr))
#define vt_prev(itr)              (__vt_itr_prev(&itr))
#define vt_ref(itr, type)         (*(type *)__vt_itr_addr(&itr))

typedef struct {
  void   *arrays[__MAX_ARRAYS_COUNT];
  size_t  arrays_count;
  size_t  type_size;
  size_t  count;
  size_t  __dimension;
} *vector, _vector;

typedef struct {
  vector  __self;
  size_t  __arrays_idx;
  size_t  __array_idx;
  size_t  idx;
} vt_itr;

#ifndef __VECTOR_MULTITHREAD
void    __vt_stack_in(void);
void    __vt_stack_out(void);
#endif
void    __vt_delete(vector self, int recurse);
vector  __vt_new(size_t type_size, size_t dimension, ...);
size_t  __vt_count(vector self);
size_t  __vt_type_size(vector self);
size_t  __vt_type_count(vector self);
size_t  __vt_capacity(vector self);
void    __vt_resize(vector self, size_t count);
void    __vt_clear(vector self);
void    __vt_add(vector self, void *item);
void    __vt_push(vector self, ...);
void    __vt_pop(vector self);
void   *__vt_at(vector self, size_t idx);
void   *__vt_front(vector self);
void   *__vt_back(vector self);
vector  __vt_move(vector *src);
vector  __vt_clone(vector src);
void    __vt_swap(vector *a, vector *b);
void    __vt_reverse(vector self);
vt_itr  __vt_iterator(vector self, size_t idx);
vt_itr  __vt_begin(vector self);
vt_itr  __vt_end(vector self);
void    __vt_itr_next(vt_itr *itr);
void    __vt_itr_prev(vt_itr *itr);
void   *__vt_itr_addr(vt_itr *itr);

#endif // __VECTOR_VECTOR_H
