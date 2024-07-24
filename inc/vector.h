#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <stddef.h>

#define MAX_VECTOR_SIZE   (1ul << 30)
#define MAX_VECTORS_COUNT (1ul << 20)
#define MAX_STACK_HEIGHT  (1ul << 20)
#define MAX_ARRAYS_COUNT  30ul
#define MAX_ALLOC_SIZE    (1ul << (MAX_ARRAYS_COUNT - 1ul))
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
# define vt_in                    __vt_stack_in   // in single-threading.
# define vt_out                   __vt_stack_out  // in single-threading.
#else
# define vt_delete(vt)            (__vt_delete(vt))  // in multi-threading.
#endif
#define vt_new(count, type)       (__vt_new(count, sizeof(type)))
#define vt_count(vt)              (__vt_count(vt))
#define vt_size(vt)               vt_count(vt)
#define vt_len(vt)                vt_count(vt)
#define vt_empty(vt)              (vt_count(vt) == 0)
#define vt_type_size(vt)          (__vt_type_size(vt))
#define vt_type_count(vt)         (__vt_type_count(vt))
#define vt_capacity(vt)           (__vt_capacity(vt))
#define vt_resize(vt, count)      (__vt_resize(vt, count))
#define vt_clear(vt)              vt_resize(vt, 0)
#define vt_add(vt, item)          (__vt_add(vt, &item)) // variable.
#define vt_push(vt, item)         (__vt_push(vt, item)) // literal value.
#define vt_pop(vt)                (__vt_pop(vt))
#define vt_at(vt, idx, type)      (*(type *)__vt_at(vt, idx))
#define vt_front(vt, type)        (*(type *)__vt_front(vt))
#define vt_back(vt, type)         (*(type *)__vt_back(vt))
#define vt_move(src)              (__vt_move(&src))
#define vt_clone(src, depth)      (__vt_clone(src, depth))
#define vt_swap(dst, src)         (__vt_swap(&dst, &src))
#define vt_reverse(vt)            (__vt_reverse(vt))
#define vt_itr(vt, idx)           (__vt_iterator(vt, idx))
#define vt_begin(vt)              (__vt_begin(vt))
#define vt_end(vt)                (__vt_end(vt))
#define vt_next(itr)              (__vt_itr_next(&itr))
#define vt_prev(itr)              (__vt_itr_prev(&itr))
#define vt_ref(itr, type)         (*(type *)__vt_itr_addr(&itr))

// // #define vt_clear(vt, depth)         (__vt_clear(vt, depth))
// #define vt_add(vt, item)            (__vt_add(vt, &item)) // variable.
// #define vt_push(vt, item)           (__vt_push(vt, item)) // literal.
// // #define vt_pop(vt, depth)           (__vt_pop(vt, depth))
// // #define vt_insert(vt, idx, item)    (__vt_insert(vt, idx, &item))  // variable.
// // #define vt_input(vt, idx, item)     (__vt_input(vt, idx, item))    // literal.
// // #define vt_erase(vt, idx, depth)    (__vt_erase(vt, idx, depth))

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
void    __vt_delete(vector self);
vector  __vt_new(size_t count, size_t type_size);
size_t  __vt_count(vector self);
size_t  __vt_type_size(vector self);
size_t  __vt_type_count(vector self);
size_t  __vt_capacity(vector self);
int     __vt_resize(vector self, size_t count);
int     __vt_add(vector self, void *item);
int     __vt_push(vector self, ...);
void    __vt_pop(vector self);
void   *__vt_at(vector self, size_t idx);
void   *__vt_front(vector self);
void   *__vt_back(vector self);
vector  __vt_move(vector *src);
vector  __vt_clone(vector src, size_t depth);
void    __vt_swap(vector *a, vector *b);
void    __vt_reverse(vector self);
vt_itr  __vt_iterator(vector self, size_t idx);
vt_itr  __vt_begin(vector self);
vt_itr  __vt_end(vector self);
void    __vt_itr_next(vt_itr *itr);
void    __vt_itr_prev(vt_itr *itr);
void   *__vt_itr_addr(vt_itr *itr);

#endif // VECTOR_VECTOR_H
