#include "vector.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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

static size_t __vt_maximum_bit(size_t n) {                                      // finish
  size_t maximum_bit = 0;
  if (n >> (maximum_bit + (1ull << 4))) maximum_bit += (1ull << 4);
  if (n >> (maximum_bit + (1ull << 3))) maximum_bit += (1ull << 3);
  if (n >> (maximum_bit + (1ull << 2))) maximum_bit += (1ull << 2);
  if (n >> (maximum_bit + (1ull << 1))) maximum_bit += (1ull << 1);
  if (n >> (maximum_bit + (1ull << 0))) maximum_bit += (1ull << 0);
  return maximum_bit;
}

static void   __vt_pre_incre(vector self, size_t count) {                      // finish
  size_t alloc_count = 1ull << (__vt_maximum_bit(count) + 2);
  void *new = calloc(alloc_count, self->__type_size);
  assert(new != __nullptr);
  memcpy(new, self->__array, self->__count * self->__type_size);
  if (self->__type_count) {
    free(self->__array);
  }
  self->__array = new;
  self->__type_count = alloc_count;
}

static void   __vt_post_decre(vector self) {                                    // finish
  size_t alloc_count = self->__type_count >> 2;
  if (alloc_count) {
    void *new = calloc(alloc_count, self->__type_size);
    assert(new != __nullptr);
    memcpy(new, self->__array, self->__count * self->__type_size);
    free(self->__array);
    self->__array = new;
    self->__type_count = alloc_count;
  } else {
    free(self->__array);
    self->__type_count = alloc_count;
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
    for (size_t i = 0; i < self->__count; ++i) {
      __vt_delete(vt_at(self, i, vector), recurse);
    }
  }
  if (self->__type_count) {
    free(self->__array);
  }
  free(self);
}

static vector __vt_new_recurse(
    size_t type_size, size_t dimension, size_t *counts) {                       // finish
  vector vt = (vector)calloc(1, sizeof(__vector));
  assert(vt != __nullptr);
  vt->__dimension = dimension--;
  vt->__type_size = dimension ? sizeof(__vector) : type_size;
  __vt_resize(vt, counts[dimension]);
  if (dimension) {
    for (size_t i = 0; i < vt->__count; ++i) {
      vt_at(vt, i, vector) =
          __vt_new_recurse(type_size, dimension, counts);
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
  if (!dimension || dimension > __MAX_VECTOR_DIMENSION) {
    return __nullptr;
  }
  va_list ap;
  va_start(ap, dimension);
  size_t counts[__MAX_VECTOR_DIMENSION];
  for (size_t i = dimension; i--; ) {
    counts[i] = va_arg(ap, long long);
  }
  va_end(ap);
  return __vt_new_recurse(type_size, dimension, counts);
}

void    __vt_resize(vector self, size_t count) {                                // finish
  if (self->__count < count) {
    if (self->__count >= self->__type_count) {
      __vt_pre_incre(self, count);
    }
    self->__count = count;
  } else if (self->__count > count) {
    self->__count = count;
    if (count <= self->__type_count >> 2) {
      __vt_post_decre(self);
    }
  }
}

void    __vt_clear(vector self) {                                               // finish
  if (self->__dimension > 1) {
    for (size_t i = 0; i < self->__count; ++i) {
      __vt_clear(vt_at(self, i, vector));
    }
  }
  free(self->__array);
  self->__type_count = 0;
  self->__count = 0;
}

void    __vt_add(vector self, void *item) {                                     // finish
  if (self->__count >= self->__type_count) {
    __vt_pre_incre(self, self->__count + 1);
  }
  ++self->__count;
  memcpy(self->__array + (self->__count - 1) * self->__type_size,
         item,
         self->__type_size);
}

void    __vt_push(vector self, ...) {                                           // finish
  va_list ap;
  va_start(ap, self);
  long long p = va_arg(ap, long long);
  va_end(ap);
  __vt_add(self, &p);
}

void    __vt_pop(vector self) {                                                 // finish
  assert(self->__count > 0);
  if (self->__dimension > 1) {
    __vt_clear(vt_back(self, vector));
  }
  if (--self->__count <= self->__type_count >> 2) {
    __vt_post_decre(self);
  }
}

void    __vt_insert(vector self, size_t idx, void *item) {                      // finish
  assert(idx <= self->__count);
  if (idx == self->__count) {
    __vt_add(self, item);
    return;
  }
  if (self->__count >= self->__type_count) {
    __vt_pre_incre(self, self->__count + 1);
  }
  ++self->__count;
  memmove(self->__array + (idx + 1) * self->__type_size,
          self->__array + idx * self->__type_size,
          (self->__count - idx) * self->__type_size);
  memcpy(self->__array + idx * self->__type_size,
         item,
         self->__type_size);
}

void    __vt_input(vector self, size_t idx, ...) {                              // finish
  va_list ap;
  va_start(ap, idx);
  long long p = va_arg(ap, long long);
  va_end(ap);
  __vt_insert(self, idx, &p);
}

void    __vt_erase(vector self, size_t idx) {                                   // finish
  assert(idx < self->__count);
  if (idx == self->__count - 1) {
    __vt_pop(self);
    return;
  }
  if (self->__dimension > 1) {
    __vt_clear(vt_at(self, idx, vector));
  }
  memmove(self->__array + idx * self->__type_size,
          self->__array + (idx + 1) * self->__type_size,
          (--self->__count - idx) * self->__type_size);
  if (self->__count <= self->__type_count >> 2) {
    __vt_post_decre(self);
  }
}

void    __vt_swap(vector *a, vector *b) {                                       // finish
  vector c = *a; *a = *b; *b = c;
}

vector  __vt_move(vector *src) {                                                // finish
  vector vt = *src;
  *src = __nullptr;
  return vt;
}

vector  __vt_clone(vector src) {                                                // finish
  vector vt = __vt_new(0, 1, 0);
  memcpy(vt, src, sizeof(__vector));
  vt->__array = malloc(vt->__type_count * vt->__type_size);
  assert(vt->__array != __nullptr);
  if (vt->__dimension > 1) {
    for (size_t i = 0; i < vt->__count; ++i) {
      vt_at(vt, i, vector) = __vt_clone(vt_at(src, i, vector));
    }
  } else {
    memcpy(vt->__array, src->__array, vt->__count * vt->__type_size);
  }
  return vt;
}

void    __vt_reverse(vector self) {                                             // finish
  if (self->__count < 2) { return; }
  size_t l = 0, r = self->__count - 1;
  while (l < r) {
    __vt_memswap(self->__array + l++ * self->__type_size,
                 self->__array + r-- * self->__type_size,
                 self->__type_size);
  }
}

vt_itr  __vt_make_itr(vector self, size_t idx) {                               // finish
  vt_itr itr = { self, idx };
  return itr;
}
