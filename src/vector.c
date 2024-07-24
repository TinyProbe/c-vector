#include "vector.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef VECTOR_MULTITHREAD
static vector __vt_vectors[MAX_VECTORS_COUNT];
static size_t __vt_vectors_count;
static size_t __vt_stack[MAX_STACK_HEIGHT];
static size_t __vt_stack_height;
static int    __vt_is_registered_atexit;
#endif

static size_t __vt_memswap_64(void *a, void *b, size_t size) {
  size_t n = size / (sizeof(long long) * 8);
  size -= n * (sizeof(long long) * 8);
  while (n--) {
    __swap(((long long *)a)[0], ((long long *)b)[0]);
    __swap(((long long *)a)[1], ((long long *)b)[1]);
    __swap(((long long *)a)[2], ((long long *)b)[2]);
    __swap(((long long *)a)[3], ((long long *)b)[3]);
    __swap(((long long *)a)[4], ((long long *)b)[4]);
    __swap(((long long *)a)[5], ((long long *)b)[5]);
    __swap(((long long *)a)[6], ((long long *)b)[6]);
    __swap(((long long *)a)[7], ((long long *)b)[7]);
    a += sizeof(long long) * 8;
    b += sizeof(long long) * 8;
  }
  return size;
}
static size_t __vt_memswap_32(void *a, void *b, size_t size) {
  size_t n = size / (sizeof(long long) * 4);
  size -= n * (sizeof(long long) * 4);
  while (n--) {
    __swap(((long long *)a)[0], ((long long *)b)[0]);
    __swap(((long long *)a)[1], ((long long *)b)[1]);
    __swap(((long long *)a)[2], ((long long *)b)[2]);
    __swap(((long long *)a)[3], ((long long *)b)[3]);
    a += sizeof(long long) * 4;
    b += sizeof(long long) * 4;
  }
  return size;
}
static size_t __vt_memswap_16(void *a, void *b, size_t size) {
  size_t n = size / (sizeof(long long) * 2);
  size -= n * (sizeof(long long) * 2);
  while (n--) {
    __swap(((long long *)a)[0], ((long long *)b)[0]);
    __swap(((long long *)a)[1], ((long long *)b)[1]);
    a += sizeof(long long) * 2;
    b += sizeof(long long) * 2;
  }
  return size;
}
static size_t __vt_memswap_8(void *a, void *b, size_t size) {
  size_t n = size / sizeof(long long);
  size -= n * sizeof(long long);
  while (n--) {
    __swap(*(long long *)a, *(long long *)b);
    a += sizeof(long long);
    b += sizeof(long long);
  }
  return size;
}
static size_t __vt_memswap_4(void *a, void *b, size_t size) {
  size_t n = size / sizeof(int);
  size -= n * sizeof(int);
  while (n--) {
    __swap(*(int *)a, *(int *)b);
    a += sizeof(int);
    b += sizeof(int);
  }
  return size;
}
static size_t __vt_memswap_2(void *a, void *b, size_t size) {
  size_t n = size / sizeof(short);
  size -= n * sizeof(short);
  while (n--) {
    __swap(*(short *)a, *(short *)b);
    a += sizeof(short);
    b += sizeof(short);
  }
  return size;
}
static size_t __vt_memswap_1(void *a, void *b, size_t size) {
  size_t n = size / sizeof(char);
  size -= n * sizeof(char);
  while (n--) {
    __swap(*(char *)a, *(char *)b);
    a += sizeof(char);
    b += sizeof(char);
  }
  return size;
}

static void   __vt_memswap(void *a, void *b, size_t size) {
  if (size >= (1 << 6))
    size = __vt_memswap_64(a, b, size);
  if (size >= (1 << 5))
    size = __vt_memswap_32(a, b, size);
  if (size >= (1 << 4))
    size = __vt_memswap_16(a, b, size);
  if (size >= (1 << 3))
    size = __vt_memswap_8(a, b, size);
  if (size >= (1 << 2))
    size = __vt_memswap_4(a, b, size);
  if (size >= (1 << 1))
    size = __vt_memswap_2(a, b, size);
  if (size >= (1 << 0))
    size = __vt_memswap_1(a, b, size);
}

static vt_itr __vt_make_iter(
    vector self, size_t arrays_idx, size_t array_idx, size_t idx) {
  vt_itr itr = { self, arrays_idx, array_idx, idx };
  return itr;
}

static size_t __vt_arrays_idx(vector self, size_t idx) {
  size_t l = 0, r = self->arrays_count;
  while (l + 1 < r) {
    size_t m = (l + r) / 2;
    if (idx < self->arrays_counts[m]) {
      r = m;
    } else {
      l = m;
    }
  }
  return idx < self->arrays_counts[l] ? l : r;
}

static size_t __vt_back_arrays_idx(vector self) {
  size_t arrays_idx = self->arrays_count;
  while (self->count < self->arrays_counts[--arrays_idx]);
  return arrays_idx + (self->count > self->arrays_counts[arrays_idx] ? 1 : 0);
}

static size_t __vt_prev_count(vector self, size_t arrays_idx) {
  return arrays_idx ? self->arrays_counts[arrays_idx - 1] : 0;
}

static void  *__vt_idx_addr(vector self, size_t arrays_idx, size_t idx) {
  return self->arrays[arrays_idx] +
      (idx - __vt_prev_count(self, arrays_idx)) * self->type_size;
}

static int    __vt_incre_limit(vector self, size_t count) {
  if (-1ul - self->count < count ||
      self->count + count > MAX_VECTOR_SIZE ||
      self->type_size > MAX_VECTOR_SIZE ||
      (self->count + count) * self->type_size > MAX_VECTOR_SIZE) {
    return -1;
  }
  return 0;
}

static void   __vt_post_incre(vector self) {
  while (self->count > __vt_type_count(self)) {
    size_t alloc_count = __min(1ul << __min(self->arrays_count, 30),
                               self->__max_alloc_count);
    self->arrays[self->arrays_count] = malloc(alloc_count * self->type_size);
    self->arrays_counts[self->arrays_count] =
        __vt_type_count(self) + alloc_count;
    ++self->arrays_count;
  }
}

static void   __vt_post_decre(vector self) {
  while (self->arrays_count >= 3) {
    if (self->count <= self->arrays_counts[self->arrays_count - 3]) {
      free(self->arrays[--self->arrays_count]);
    } else {
      break;
    }
  }
}

#ifndef VECTOR_MULTITHREAD
static void   __vt_clean(void) {
  while (__vt_vectors_count) {
    __vt_delete(__vt_vectors[--__vt_vectors_count]);
  }
}

void    __vt_stack_in(void) {
  __vt_stack[++__vt_stack_height] = 0;
}

void    __vt_stack_out(void) {
  assert(__vt_stack_height > 0);
  while (__vt_stack[__vt_stack_height]--) {
    __vt_delete(__vt_vectors[--__vt_vectors_count]);
  }
  --__vt_stack_height;
}
#endif

void    __vt_delete(vector self) {
  while (self->arrays_count) {
    free(self->arrays[--self->arrays_count]);
  }
  free(self);
}

vector  __vt_new(size_t count, size_t type_size) {
#ifndef VECTOR_MULTITHREAD
  if (!__vt_is_registered_atexit) {
    __vt_is_registered_atexit = 1;
    atexit(__vt_clean);
  }
#endif
  vector vt = (vector)calloc(1, sizeof(_vector));
  vt->__max_alloc_count = (type_size ? MAX_ALLOC_SIZE / type_size : 0);
  vt->type_size = type_size;
  if (__vt_resize(vt, count)) {
    free(vt);
    return nullptr;
  }
#ifndef VECTOR_MULTITHREAD
  __vt_vectors[__vt_vectors_count++] = vt;
  ++__vt_stack[__vt_stack_height];
#endif
  return vt;
}

size_t  __vt_count(vector self) {
  return self->count;
}

size_t  __vt_type_size(vector self) {
  return self->type_size;
}

size_t  __vt_type_count(vector self) {
  return self->arrays_count ? self->arrays_counts[self->arrays_count - 1] : 0;
}

size_t  __vt_capacity(vector self) {
  return self->type_size * __vt_type_count(self);
}

int     __vt_resize(vector self, size_t count) {
  if (self->count < count &&
      __vt_incre_limit(self, count - self->count)) { return -1; }
  if (self->count < count) {
    self->count = count;
    __vt_post_incre(self);
  } else if (self->count > count) {
    self->count = count;
    __vt_post_decre(self);
  }
  return 0;
}

int     __vt_add(vector self, void *item) {
  if (__vt_incre_limit(self, 1)) { return -1; }
  ++self->count;
  __vt_post_incre(self);
  memcpy(__vt_back(self), item, self->type_size);
  return 0;
}

int     __vt_push(vector self, ...) {
  va_list ap;
  va_start(ap, self);
  long long p = va_arg(ap, long long);
  va_end(ap);
  return __vt_add(self, &p);
}

void    __vt_pop(vector self) {
  assert(self->count > 0);
  --self->count;
  __vt_post_decre(self);
}

void   *__vt_at(vector self, size_t idx) {
  assert(idx < self->count);
  return __vt_idx_addr(self, __vt_arrays_idx(self, idx), idx);
}

void   *__vt_front(vector self) {
  assert(self->count > 0);
  return *self->arrays;
}

void   *__vt_back(vector self) {
  assert(self->count > 0);
  return __vt_idx_addr(self, __vt_back_arrays_idx(self), self->count - 1);
}

vector  __vt_move(vector *src) {
  vector vt = *src;
  *src = nullptr;
  return vt;
}

vector  __vt_clone(vector src, size_t depth) {
  assert(depth-- > 0);
  vector vt = __vt_new(0, 0);
  memcpy(vt, src, sizeof(_vector));
  for (size_t i = 0; i < vt->arrays_count; ++i) {
    size_t capacity = vt->arrays_counts[i] * vt->type_size;
    vt->arrays[i] = malloc(capacity);
    if (!depth) {
      memcpy(vt->arrays[i], src->arrays[i], capacity);
    }
  }
  if (depth) {
    for (vt_itr i = __vt_begin(vt), j = __vt_begin(src); i.idx < vt->count;
        __vt_itr_next(&i), __vt_itr_next(&j)) {
      vt_ref(i, vector) = __vt_clone(vt_ref(j, vector), depth);
    }
  }
  return vt;
}

void    __vt_swap(vector *a, vector *b) {
  assert(a != b);
  __swap(*((long *)a), *((long *)b));
}

void    __vt_reverse(vector self) {
  if (self->count < 2) { return; }
  vt_itr l = __vt_begin(self), r = __vt_end(self);
  __vt_itr_prev(&r);
  while (l.idx < r.idx) {
    __vt_memswap(__vt_itr_addr(&l), __vt_itr_addr(&r), self->type_size);
    __vt_itr_next(&l);
    __vt_itr_prev(&r);
  }
}

vt_itr  __vt_iterator(vector self, size_t idx) {
  size_t arrays_idx = __vt_arrays_idx(self, idx);
  return __vt_make_iter(self, arrays_idx,
                        idx - __vt_prev_count(self, arrays_idx),
                        idx);
}

vt_itr  __vt_begin(vector self) {
  return __vt_make_iter(self, 0ul, 0ul, 0ul);
}

vt_itr  __vt_end(vector self) {
  size_t arrays_idx = __vt_back_arrays_idx(self);
  return __vt_make_iter(self, arrays_idx,
                        self->count - __vt_prev_count(self, arrays_idx),
                        self->count);
}

void    __vt_itr_next(vt_itr *itr) {
  assert(itr->idx < itr->__self->count);
  ++itr->idx;
  size_t current_count = itr->__self->arrays_counts[itr->__arrays_idx];
  size_t prev_count = __vt_prev_count(itr->__self, itr->__arrays_idx);
  if (++itr->__array_idx == current_count - prev_count) {
    ++itr->__arrays_idx;
    itr->__array_idx = 0;
  }
}

void    __vt_itr_prev(vt_itr *itr) {
  assert(itr->idx > 0);
  --itr->idx;
  if (itr->__array_idx-- == 0) {
    size_t current_count = itr->__self->arrays_counts[--itr->__arrays_idx];
    size_t prev_count = __vt_prev_count(itr->__self, itr->__arrays_idx);
    itr->__array_idx = current_count - prev_count - 1;
  }
}

void   *__vt_itr_addr(vt_itr *itr) {
  vector self = itr->__self;
  return self->arrays[itr->__arrays_idx] + itr->__array_idx * self->type_size;
}
