#include "vector.h"

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
  if (size >= (1 << 6)) __vt_memswap_64(&a, &b, &size);
  if (size >= (1 << 5)) __vt_memswap_32(&a, &b, &size);
  if (size >= (1 << 4)) __vt_memswap_16(&a, &b, &size);
  if (size >= (1 << 3)) __vt_memswap_8(&a, &b, &size);
  if (size >= (1 << 2)) __vt_memswap_4(&a, &b, &size);
  if (size >= (1 << 1)) __vt_memswap_2(&a, &b, &size);
  if (size >= (1 << 0)) __vt_memswap_1(&a, &b, &size);
}

static void   __vt_memmove(vector self, size_t idx, long long offset) {         // edit
  (void)self;
  (void)idx;
  (void)offset;
}

static vt_itr __vt_make_iter(                                                   // finish
    vector self, size_t arrays_idx, size_t array_idx, size_t idx) {
  vt_itr itr = { self, arrays_idx, array_idx, idx };
  return itr;
}

static size_t __vt_arrays_idx(vector self, size_t idx) {                        // optimize
  size_t arrays_idx = self->arrays_count / 2;
  while (idx >= self->arrays_counts[arrays_idx]) { ++arrays_idx; }
  while (idx + 1 < self->arrays_counts[arrays_idx]) { --arrays_idx; }
  return arrays_idx + (idx >= self->arrays_counts[arrays_idx] ? 1 : 0);
}

static size_t __vt_back_arrays_idx(vector self) {                               // optimize
  size_t arrays_idx = self->arrays_count;
  while (self->count < self->arrays_counts[--arrays_idx]);
  return arrays_idx + (self->count > self->arrays_counts[arrays_idx] ? 1 : 0);
}

static size_t __vt_prev_count(vector self, size_t arrays_idx) {                 // finish
  return arrays_idx ? self->arrays_counts[arrays_idx - 1] : 0;
}

static void  *__vt_idx_addr(vector self, size_t arrays_idx, size_t idx) {       // finish
  return self->arrays[arrays_idx] +
      (idx - __vt_prev_count(self, arrays_idx)) * self->type_size;
}

static int    __vt_incre_limit(vector self, size_t count) {                     // finish
  if (count > __MAX_VECTOR_SIZE ||
      self->type_size > __MAX_VECTOR_SIZE ||
      count * self->type_size > __MAX_VECTOR_SIZE) { return -1; }
  return 0;
}

static void   __vt_post_incre(vector self) {                                    // finish
  while (self->count > __vt_type_count(self)) {
    size_t alloc_count = __min(1ul << __min(self->arrays_count, 30),
                              self->__max_alloc_count);
    self->arrays[self->arrays_count] = calloc(alloc_count, self->type_size);
    self->arrays_counts[self->arrays_count] =
        __vt_type_count(self) + alloc_count;
    ++self->arrays_count;
  }
}

static void   __vt_post_decre(vector self) {                                    // finish
  while (self->arrays_count >= 3) {
    if (self->count <= self->arrays_counts[self->arrays_count - 3]) {
      free(self->arrays[--self->arrays_count]);
    } else {
      break;
    }
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
  while (self->arrays_count) {
    free(self->arrays[--self->arrays_count]);
  }
  free(self);
}

static vector __vt_new_recurse(
    size_t type_size, size_t dimension, size_t *counts) {                       // finish
  vector vt = (vector)calloc(1, sizeof(_vector));
  vt->__dimension = dimension--;
  vt->__max_alloc_count = dimension ? __MAX_ALLOC_SIZE / sizeof(_vector)
      : (type_size ? __MAX_ALLOC_SIZE / type_size : 0);
  vt->type_size = dimension ? sizeof(_vector) : type_size;
  if (__vt_resize(vt, counts[dimension])) {
    free(vt);
    return __nullptr;
  }
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
    __vt_is_registered_atexit = 1;
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
  return self->arrays_count ? self->arrays_counts[self->arrays_count - 1] : 0;
}

size_t  __vt_capacity(vector self) {                                            // finish
  return self->type_size * __vt_type_count(self);
}

int     __vt_resize(vector self, size_t count) {                                // finish
  if (self->count < count && __vt_incre_limit(self, count)) { return -1; }
  if (self->count < count) {
    self->count = count;
    __vt_post_incre(self);
  } else if (self->count > count) {
    self->count = count;
    __vt_post_decre(self);
  }
  return 0;
}

void    __vt_clear(vector self) {                                               // finish
  if (self->__dimension > 1) {
    for (vt_itr i = __vt_begin(self); i.idx < self->count;
        __vt_itr_next(&i)) {
      __vt_clear(vt_ref(i, vector));
    }
  }
  self->count = 0;
  while (self->arrays_count) {
    free(self->arrays[--self->arrays_count]);
  }
}

int     __vt_add(vector self, void *item) {                                     // finish
  if (__vt_incre_limit(self, self->count + 1)) { return -1; }
  ++self->count;
  __vt_post_incre(self);
  memcpy(__vt_back(self), item, self->type_size);
  return 0;
}

int     __vt_push(vector self, ...) {                                           // finish
  va_list ap;
  va_start(ap, self);
  long long p = va_arg(ap, long long);
  va_end(ap);
  return __vt_add(self, &p);
}

void    __vt_pop(vector self) {                                                 // finish
  assert(self->count > 0);
  if (self->__dimension > 1) {
    __vt_clear(vt_back(self, vector));
  }
  --self->count;
  __vt_post_decre(self);
}

int     __vt_insert(vector self, size_t idx, void *item) {                      // finish
  if (__vt_incre_limit(self, self->count + 1)) { return -1; }
  ++self->count;
  __vt_post_incre(self);
  __vt_memmove(self, idx, 1);
  memcpy(__vt_idx_addr(self, __vt_arrays_idx(self, idx), idx),
         item, self->type_size);
  return 0;
}

int     __vt_input(vector self, size_t idx, ...) {                              // finish
  va_list ap;
  va_start(ap, idx);
  long long p = va_arg(ap, long long);
  va_end(ap);
  return __vt_insert(self, idx, &p);
}

void    __vt_erase(vector self, size_t idx) {                                   // finish
  assert(idx < self->count);
  __vt_memmove(self, idx + 1, -1);
  --self->count;
  __vt_post_decre(self);
}

void   *__vt_at(vector self, size_t idx) {                                      // finish
  assert(idx < self->count);
  return __vt_idx_addr(self, __vt_arrays_idx(self, idx), idx);
}

void   *__vt_front(vector self) {                                               // finish
  assert(self->count > 0);
  return *self->arrays;
}

void   *__vt_back(vector self) {                                                // finish
  assert(self->count > 0);
  return __vt_idx_addr(self, __vt_back_arrays_idx(self), self->count - 1);
}

vector  __vt_move(vector *src) {                                                // finish
  vector vt = *src;
  *src = __nullptr;
  return vt;
}

vector  __vt_clone(vector src) {                                                // finish
  vector vt = __vt_new(0, 1, 0);
  memcpy(vt, src, sizeof(_vector));
  for (size_t i = 0; i < vt->arrays_count; ++i) {
    size_t capacity = vt->arrays_counts[i] * vt->type_size;
    vt->arrays[i] = malloc(capacity);
    if (vt->__dimension == 1) {
      memcpy(vt->arrays[i], src->arrays[i], capacity);
    }
  }
  if (vt->__dimension > 1) {
    for (vt_itr i = __vt_begin(vt), j = __vt_begin(src); i.idx < vt->count;
        __vt_itr_next(&i), __vt_itr_next(&j)) {
      vt_ref(i, vector) = __vt_clone(vt_ref(j, vector));
    }
  }
  return vt;
}

void    __vt_swap(vector *a, vector *b) {                                       // finish
  vector c = *a; *a = *b; *b = c;
}

void    __vt_reverse(vector self) {                                             // finish
  if (self->count < 2) { return; }
  vt_itr l = __vt_begin(self), r = __vt_end(self);
  __vt_itr_prev(&r);
  while (l.idx < r.idx) {
    __vt_memswap(__vt_itr_addr(&l), __vt_itr_addr(&r), self->type_size);
    __vt_itr_next(&l);
    __vt_itr_prev(&r);
  }
}

vt_itr  __vt_iterator(vector self, size_t idx) {                                // finish
  size_t arrays_idx = __vt_arrays_idx(self, idx);
  return __vt_make_iter(self, arrays_idx,
                        idx - __vt_prev_count(self, arrays_idx),
                        idx);
}

vt_itr  __vt_begin(vector self) {                                               // finish
  return __vt_make_iter(self, 0ul, 0ul, 0ul);
}

vt_itr  __vt_end(vector self) {                                                 // finish
  size_t arrays_idx = __vt_back_arrays_idx(self);
  return __vt_make_iter(self, arrays_idx,
                        self->count - __vt_prev_count(self, arrays_idx),
                        self->count);
}

void    __vt_itr_next(vt_itr *itr) {                                            // finish
  assert(itr->idx < itr->__self->count);
  ++itr->idx;
  size_t current_count = itr->__self->arrays_counts[itr->__arrays_idx];
  size_t prev_count = __vt_prev_count(itr->__self, itr->__arrays_idx);
  if (++itr->__array_idx == current_count - prev_count) {
    ++itr->__arrays_idx;
    itr->__array_idx = 0;
  }
}

void    __vt_itr_prev(vt_itr *itr) {                                            // finish
  assert(itr->idx > 0);
  --itr->idx;
  if (itr->__array_idx-- == 0) {
    size_t current_count = itr->__self->arrays_counts[--itr->__arrays_idx];
    size_t prev_count = __vt_prev_count(itr->__self, itr->__arrays_idx);
    itr->__array_idx = current_count - prev_count - 1;
  }
}

void   *__vt_itr_addr(vt_itr *itr) {                                            // finish
  vector self = itr->__self;
  return self->arrays[itr->__arrays_idx] + itr->__array_idx * self->type_size;
}
