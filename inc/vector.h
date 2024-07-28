#ifndef __VECTOR_VECTOR_H
#define __VECTOR_VECTOR_H

#include <stddef.h>
#include <assert.h>

#define __MAX_VECTORS_COUNT     (1ull << 20)
#define __MAX_STACK_HEIGHT      (1ull << 20)
#define __MAX_VECTOR_DIMENSION  (1ull << 5)

#define __nullptr     (void *)0
#define __true        1
#define __false       0

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
#define vt_count(vt)              (vt->__count)
#define vt_size(vt)               vt_count(vt)
#define vt_len(vt)                vt_count(vt)
#define vt_empty(vt)              (vt_count(vt) == 0)
#define vt_type_size(vt)          (vt->__type_size)
#define vt_type_count(vt)         (vt->__type_count)
#define vt_capacity(vt)           (vt_type_size(vt) * vt_type_count(vt))
#define vt_resize(vt, count)      (__vt_resize(vt, count))
#define vt_clear(vt)              (__vt_clear(vt))
#define vt_add(vt, item)          (__vt_add(vt, &item))   // variable.
#define vt_push(vt, item)         (__vt_push(vt, item))   // literal.
#define vt_pop(vt)                (__vt_pop(vt))
#define vt_insert(vt, idx, item)  (__vt_insert(vt, idx, &item)) // variable.
#define vt_input(vt, idx, item)   (__vt_input(vt, idx, item))   // literal.
#define vt_erase(vt, idx)         (__vt_erase(vt, idx))
#define vt_at(vt, idx, type)      ((type *)vt->__array)[idx]
#define vt_front(vt, type)        ((type *)vt->__array)[0]
#define vt_back(vt, type)         ((type *)vt->__array)[vt->__count-1]
#define vt_swap(l, r)             (__vt_swap(&l, &r))
#define vt_move(src)              (__vt_move(&src))
#define vt_clone(src)             (__vt_clone(src))
#define vt_reverse(vt)            (__vt_reverse(vt))
#define vt_itr(vt, idx)           (__vt_make_itr(vt, idx))
#define vt_begin(vt)              (__vt_make_itr(vt, 0))
#define vt_end(vt)                (__vt_make_itr(vt, vt->__count-1))
#define vt_next(itr)              (assert(itr.idx++ < itr.__self->__count))
#define vt_prev(itr)              (assert(itr.idx-- > 0))
#define vt_ref(itr, type)         ((type *)itr.__self->__array)[itr.idx]

typedef struct {
  void   *__array;
  size_t  __type_size;
  size_t  __type_count;
  size_t  __count;
  size_t  __dimension;
} *vector, __vector;

typedef struct {
  vector  __self;
  size_t  idx;
} vt_itr;

#ifndef __VECTOR_MULTITHREAD
void    __vt_stack_in(void);
void    __vt_stack_out(void);
#endif
void    __vt_delete(vector self, int recurse);
vector  __vt_new(size_t type_size, size_t dimension, ...);
void    __vt_resize(vector self, size_t count);
void    __vt_clear(vector self);
void    __vt_add(vector self, void *item);
void    __vt_push(vector self, ...);
void    __vt_pop(vector self);
void    __vt_insert(vector self, size_t idx, void *item);
void    __vt_input(vector self, size_t idx, ...);
void    __vt_erase(vector self, size_t idx);
void    __vt_swap(vector *l, vector *r);
vector  __vt_move(vector *src);
vector  __vt_clone(vector src);
void    __vt_reverse(vector self);
vt_itr  __vt_make_itr(vector self, size_t idx);

#endif // __VECTOR_VECTOR_H
