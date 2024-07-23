//
//  WARNING: Multidimensional vector not supported.
//  TODO: Multidimensional vector support.
//
#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <stddef.h>

#define MAX_ALLOC_SIZE    16711568
#define MAX_ARRAYS_COUNT  (1ul << 8)
#define MAX_VECTOR_SIZE   -1u
#define MAX_VECTORS_COUNT (1ul << 20)
#define MAX_STACK_HEIGHT  (1ul << 20)
#ifndef nullptr
# define nullptr          (void *)0
#endif

#define __min(a, b)   ((a)<(b) ? a : b)
#define __max(a, b)   ((a)>(b) ? a : b)
#define __swap(a, b)  ((a)^=(b),(b)^=(a),(a)^=(b))

// #define VECTOR_MULTITHREAD
//
//  When using multithreading with vectors.
//  VECTOR_MULTITHREAD must be defined.
//
#ifndef VECTOR_MULTITHREAD
# define vt_in                  __vt_stack_in   // Use in single-threading.
# define vt_out                 __vt_stack_out  // Use in single-threading.
#else
# define vt_delete(vt)          (__vt_delete(vt))  // Use in multi-threading.
#endif
#define vt_new(count, type)     (__vt_new(count, sizeof(type)))
#define vt_count(vt)            (__vt_count(vt))
#define vt_size(vt)             vt_count(vt)
#define vt_length(vt)           vt_count(vt)
#define vt_empty(vt)            (vt_count(vt) == 0)
#define vt_type_size(vt)        (__vt_type_size(vt))
#define vt_type_count(vt)       (__vt_type_count(vt))
#define vt_capacity(vt)         (__vt_capacity(vt))
#define vt_resize(vt, count)    (__vt_resize(vt, count))
#define vt_clear(vt)            vt_resize(vt, 0)
#define vt_add(vt, item)        (__vt_add(vt, &item)) // variable.
#define vt_push(vt, item)       (__vt_push(vt, item)) // literal/pointer.
#define vt_pop(vt)              (__vt_pop(vt))
#define vt_at(vt, idx, type)    (*(type *)__vt_at(vt, idx)) // O(log(log(n))).
#define vt_front(vt, type)      (*(type *)__vt_front(vt))
#define vt_back(vt, type)       (*(type *)__vt_back(vt))
#define vt_move(dst, src)       (__vt_move(&dst, &src))
#define vt_copy(dst, src)       (__vt_copy(&dst, &src))
#define vt_swap(dst, src)       (__vt_swap(&dst, &src))
#define vt_reverse(vt)          (__vt_reverse(vt))
#define vt_iterator(vt, idx)    (__vt_iterator(vt, idx))    // O(log(log(n))).
#define vt_begin(vt)            (__vt_begin(vt))
#define vt_end(vt)              (__vt_end(vt))
#define vt_next(itr)            (__vt_itr_next(&itr))
#define vt_prev(itr)            (__vt_itr_prev(&itr))
#define vt_deref(itr, type)     (*(type *)__vt_itr_addr(&itr))

typedef struct {
  void   *arrays[MAX_ARRAYS_COUNT];
  size_t  arrays_counts[MAX_ARRAYS_COUNT];
  size_t  arrays_count;
  size_t  type_size;
  size_t  count;
  size_t  __max_alloc_count;
} *vector, _vector;

typedef struct {
  vector  __self;
  size_t  __arrays_idx;
  size_t  __array_idx;
  size_t  idx;
} vt_itr;

#ifndef VECTOR_MULTITHREAD
void    __vt_stack_in(void);
void    __vt_stack_out(void);
#endif
void    __vt_delete(vector self);               // !recursive.
vector  __vt_new(size_t count, size_t type_size);
size_t  __vt_count(vector self);
size_t  __vt_type_size(vector self);
size_t  __vt_type_count(vector self);
size_t  __vt_capacity(vector self);
int     __vt_resize(vector self, size_t count); // !recursive: __vt_post_decre.
int     __vt_add(vector self, void *item);
int     __vt_push(vector self, ...);
void    __vt_pop(vector self);                  // !recursive: __vt_post_decre.
void   *__vt_at(vector self, size_t idx);
void   *__vt_front(vector self);
void   *__vt_back(vector self);
void    __vt_move(vector *dst, vector *src);
void    __vt_copy(vector *dst, vector *src);    // !recursive.
void    __vt_swap(vector *a, vector *b);
void    __vt_reverse(vector self);
vt_itr  __vt_iterator(vector self, size_t idx);
vt_itr  __vt_begin(vector self);
vt_itr  __vt_end(vector self);
void    __vt_itr_next(vt_itr *itr);
void    __vt_itr_prev(vt_itr *itr);
void   *__vt_itr_addr(vt_itr *itr);

#endif // VECTOR_VECTOR_H
