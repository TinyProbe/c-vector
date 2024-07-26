#include "vector.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef __VECTOR_MULTITHREAD
static vector __vt_vectors[__MAX_VECTORS_COUNT];
static size_t __vt_vectors_count;
static size_t __vt_stack[__MAX_STACK_HEIGHT];
static size_t __vt_stack_height;
static int    __vt_is_registered_atexit;
#endif

static void   __vt_memswap_64(void **a, void **b, size_t *size) {               // finish
  size_t n = *size / (sizeof(long long) * 8);
  *size -= n * (sizeof(long long) * 8);
  long long c[8];
  while (n--) {
    memcpy(c, *a, sizeof(long long) * 8);
    memcpy(*a, *b, sizeof(long long) * 8);
    memcpy(*b, c, sizeof(long long) * 8);
    *a += sizeof(long long) * 8;
    *b += sizeof(long long) * 8;
  }
}
static void   __vt_memswap_32(void **a, void **b, size_t *size) {               // finish
  *size -= sizeof(long long) * 4;
  long long c[4];
  memcpy(c, *a, sizeof(long long) * 4);
  memcpy(*a, *b, sizeof(long long) * 4);
  memcpy(*b, c, sizeof(long long) * 4);
  *a += sizeof(long long) * 4;
  *b += sizeof(long long) * 4;
}
static void   __vt_memswap_16(void **a, void **b, size_t *size) {               // finish
  *size -= sizeof(long long) * 2;
  long long c[2];
  memcpy(c, *a, sizeof(long long) * 2);
  memcpy(*a, *b, sizeof(long long) * 2);
  memcpy(*b, c, sizeof(long long) * 2);
  *a += sizeof(long long) * 2;
  *b += sizeof(long long) * 2;
}
static void   __vt_memswap_8(void **a, void **b, size_t *size) {                // finish
  *size -= sizeof(long long);
  long long c = *(long long *)*a;
  *(long long *)*a = *(long long *)*b;
  *(long long *)*b = c;
  *a += sizeof(long long);
  *b += sizeof(long long);
}
static void   __vt_memswap_4(void **a, void **b, size_t *size) {                // finish
  *size -= sizeof(int);
  int c = *(int *)*a;
  *(int *)*a = *(int *)*b;
  *(int *)*b = c;
  *a += sizeof(int);
  *b += sizeof(int);
}
static void   __vt_memswap_2(void **a, void **b, size_t *size) {                // finish
  *size -= sizeof(short);
  short c = *(short *)*a;
  *(short *)*a = *(short *)*b;
  *(short *)*b = c;
  *a += sizeof(short);
  *b += sizeof(short);
}
static void   __vt_memswap_1(void **a, void **b, size_t *size) {                // finish
  *size -= sizeof(char);
  char c = *(char *)*a;
  *(char *)*a = *(char *)*b;
  *(char *)*b = c;
  *a += sizeof(char);
  *b += sizeof(char);
}
static void   __vt_memswap(void *a, void *b, size_t size) {                     // finish
  if (size >= (1ull << 6)) __vt_memswap_64(&a, &b, &size);
  if (size >= (1ull << 5)) __vt_memswap_32(&a, &b, &size);
  if (size >= (1ull << 4)) __vt_memswap_16(&a, &b, &size);
  if (size >= (1ull << 3)) __vt_memswap_8(&a, &b, &size);
  if (size >= (1ull << 2)) __vt_memswap_4(&a, &b, &size);
  if (size >= (1ull << 1)) __vt_memswap_2(&a, &b, &size);
  if (size >= (1ull << 0)) __vt_memswap_1(&a, &b, &size);
}

static vt_itr __vt_make_iter(vector self, size_t idx) {                         // finish
  vt_itr itr = { self, idx };
  return itr;
}

// static size_t __vt_arrays_counts(size_t idx) {                                  // edit
//   return (1ull << (idx + 1)) - 1;
// }

static size_t __vt_maximum_bit(size_t idx) {                                    // edit
  size_t arrays_idx = 0;
  ++idx;
  if (idx >> 16) { arrays_idx += 16; }
  if (idx >> (arrays_idx + 8)) { arrays_idx += 8; }
  if (idx >> (arrays_idx + 4)) { arrays_idx += 4; }
  if (idx >> (arrays_idx + 2)) { arrays_idx += 2; }
  if (idx >> (arrays_idx + 1)) { arrays_idx += 1; }
  return arrays_idx;
}

// static size_t __vt_back_arrays_idx(vector self) {                               // edit
//   if (self->arrays_count >= 2) {
//     return self->count > __vt_arrays_counts(self->arrays_count - 2)
//         ? self->arrays_count - 1
//         : self->arrays_count - 2;
//   }
//   return 0;
// }

// static size_t __vt_prev_count(size_t arrays_idx) {                              // edit
//   return arrays_idx ? __vt_arrays_counts(arrays_idx - 1) : 0;
// }

// static void  *__vt_idx_addr(vector self, size_t arrays_idx, size_t idx) {       // edit
//   return self->arrays[arrays_idx] +
//       (idx - __vt_prev_count(arrays_idx)) * self->type_size;
// }

static void   __vt_post_incre(vector self) {                                    // finish
  if (self->count > self->type_count) {
    size_t alloc_count = 1ull << (__vt_maximum_bit(self->count) + 1);
    void *new = calloc(alloc_count, self->type_size);
    assert(new != __nullptr);
    memcpy(new, self->array, self->type_count * self->type_size);
    free(self->array);
    self->array = new;
    self->type_count = alloc_count;
  }
}

static void   __vt_post_decre(vector self) {                                    // finish
  if (self->count < self->type_count >> 1) {
    size_t alloc_count = 1ull << (__vt_maximum_bit(self->count) + 1);
    void *new = calloc(alloc_count, self->type_size);
    assert(new != __nullptr);
    memcpy(new, self->array, self->type_count * self->type_size);
    free(self->array);
    self->array = new;
    self->type_count = alloc_count;
  }
}

#ifndef __VECTOR_MULTITHREAD
static void   __vt_clean(void) {                                                // finish
  while (__vt_vectors_count) {
    __vt_delete(__vt_vectors[--__vt_vectors_count], __false);
  }
}

void    __vt_stack_in(void) {                                                   // finish
  __vt_stack[++__vt_stack_height] = 0;
}

void    __vt_stack_out(void) {                                                  // finish
  assert(__vt_stack_height > 0);
  while (__vt_stack[__vt_stack_height]--) {
    __vt_delete(__vt_vectors[--__vt_vectors_count], __false);
  }
  --__vt_stack_height;
}
#endif

void    __vt_delete(vector self, int recurse) {                                 // finish
  if (recurse && self->__dimension > 1) {
    for (vt_itr i = __vt_begin(self); i.idx < self->count;
        __vt_itr_next(&i)) {
      __vt_delete(vt_ref(i, vector), recurse);
    }
  }
  if (self->type_count) {
    free(self->array);
  }
  free(self);
}

static vector __vt_new_recurse(
    size_t type_size, size_t dimension, size_t *counts) {                       // finish
  vector vt = (vector)calloc(1, sizeof(__vector));
  assert(vt != __nullptr);
  vt->__dimension = dimension--;
  vt->type_size = dimension ? sizeof(__vector) : type_size;
  __vt_resize(vt, counts[dimension]);
  if (dimension) {
    for (vt_itr i = __vt_begin(vt); i.idx < vt->count;
        __vt_itr_next(&i)) {
      vt_ref(i, vector) = __vt_new_recurse(type_size, dimension, counts);
    }
  }
#ifndef __VECTOR_MULTITHREAD
  __vt_vectors[__vt_vectors_count++] = vt;
  ++__vt_stack[__vt_stack_height];
#endif
  return vt;
}

vector  __vt_new(size_t type_size, size_t dimension, ...) {                     // finish
#ifndef __VECTOR_MULTITHREAD
  if (!__vt_is_registered_atexit) {
    __vt_is_registered_atexit = __true;
    atexit(__vt_clean);
  }
#endif
  if (!dimension || dimension > __MAX_VECTOR_DIMENSION) { return __nullptr; }
  va_list ap;
  va_start(ap, dimension);
  size_t counts[__MAX_VECTOR_DIMENSION];
  for (size_t i = dimension; i--; ) {
    counts[i] = va_arg(ap, long long);
  }
  va_end(ap);
  return __vt_new_recurse(type_size, dimension, counts);
}

size_t  __vt_count(vector self) {                                               // finish
  return self->count;
}

size_t  __vt_type_size(vector self) {                                           // finish
  return self->type_size;
}

size_t  __vt_type_count(vector self) {                                          // finish
  return self->type_count;
}

size_t  __vt_capacity(vector self) {                                            // finish
  return self->type_size * self->type_count;
}

void    __vt_resize(vector self, size_t count) {                                // finish
  if (self->count < count) {
    self->count = count;
    __vt_post_incre(self);
  } else if (self->count > count) {
    self->count = count;
    __vt_post_decre(self);
  }
}

void    __vt_clear(vector self) {                                               // finish
  if (self->__dimension > 1) {
    for (vt_itr i = __vt_begin(self); i.idx < self->count;
        __vt_itr_next(&i)) {
      __vt_clear(vt_ref(i, vector));
    }
  }
  free(self->array);
  self->type_count = 0;
  self->count = 0;
}

void    __vt_add(vector self, void *item) {                                     // finish
  ++self->count;
  __vt_post_incre(self);
  memcpy(__vt_back(self), item, self->type_size);
}

void    __vt_push(vector self, ...) {                                           // finish
  va_list ap;
  va_start(ap, self);
  long long p = va_arg(ap, long long);
  va_end(ap);
  __vt_add(self, &p);
}

void    __vt_pop(vector self) {                                                 // finish
  assert(self->count > 0);
  if (self->__dimension > 1) {
    __vt_clear(vt_back(self, vector));
  }
  --self->count;
  __vt_post_decre(self);
}

void   *__vt_at(vector self, size_t idx) {                                      // finish
  assert(idx < self->count);
  return self->array + idx * self->type_size;
}

void   *__vt_front(vector self) {                                               // finish
  assert(self->count > 0);
  return self->array;
}

void   *__vt_back(vector self) {                                                // finish
  assert(self->count > 0);
  return self->array + (self->count - 1) * self->type_size;
}

vector  __vt_move(vector *src) {                                                // finish
  vector vt = *src;
  *src = __nullptr;
  return vt;
}

vector  __vt_clone(vector src) {                                                // finish
  vector vt = __vt_new(0, 1, 0);
  memcpy(vt, src, sizeof(__vector));
  vt->array = malloc(src->type_count * src->type_size);
  assert(vt->array != __nullptr);
  if (vt->__dimension == 1) {
    memcpy(vt->array, src->array, vt->count * vt->type_size);
  }
  if (vt->__dimension > 1) {
    for (size_t i = 0; i < vt->count; ++i) {
      vt_at(vt, i, vector) = __vt_clone(vt_at(src, i, vector));
    }
  }
  return vt;
}

void    __vt_swap(vector *a, vector *b) {                                       // finish
  vector c = *a; *a = *b; *b = c;
}

void    __vt_reverse(vector self) {                                             // finish
  if (self->count < 2) { return; }
  size_t l = 0, r = self->count - 1;
  while (l < r) {
    __vt_memswap(
        __vt_at(self, l++), __vt_at(self, r--), self->type_size);
  }
}

vt_itr  __vt_iterator(vector self, size_t idx) {                                // finish
  return __vt_make_iter(self, idx);
}

vt_itr  __vt_begin(vector self) {                                               // finish
  return __vt_make_iter(self, 0ul);
}

vt_itr  __vt_end(vector self) {                                                 // finish
  return __vt_make_iter(self, self->count);
}

void    __vt_itr_next(vt_itr *itr) {                                            // finish
  assert(itr->idx < itr->__self->count);
  ++itr->idx;
}

void    __vt_itr_prev(vt_itr *itr) {                                            // finish
  assert(itr->idx > 0);
  --itr->idx;
}

void   *__vt_itr_addr(vt_itr *itr) {                                            // finish
  return __vt_at(itr->__self, itr->idx);
}
