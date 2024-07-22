#include "vector.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static vector __vt_vectors[MAX_VECTORS_COUNT];
static size_t __vt_vectors_count;
static size_t __vt_stack[MAX_STACK_HEIGHT];
static size_t __vt_stack_height;
static int    __vt_is_registered_atexit;

static size_t __vt_array_idx(vector self, size_t idx) {
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
    size_t alloc_count = __vt_min(1ul << __vt_min(self->arrays_count, 30),
                                  self->__max_alloc_count);
    self->arrays[self->arrays_count] = malloc(self->type_size * alloc_count);
    self->arrays_counts[self->arrays_count] =
        __vt_type_count(self) + alloc_count;
    ++self->arrays_count;
  }
}

static void   __vt_post_decre(vector self) {
  while (self->arrays_count > 2) {
    if (self->count <= self->arrays_counts[self->arrays_count - 3]) {
      free(self->arrays[--self->arrays_count]);
    } else {
      break;
    }
  }
}

static void   __vt_clean(void) {
  while (__vt_vectors_count) {
    __vt_delete(__vt_vectors[--__vt_vectors_count]);
  }
}

void    __vt_scope_begin(void) {
  __vt_stack[++__vt_stack_height] = 0;
}

void    __vt_scope_end(void) {
  assert(__vt_stack_height > 0);
  while (__vt_stack[__vt_stack_height]--) {
    __vt_delete(__vt_vectors[--__vt_vectors_count]);
  }
  --__vt_stack_height;
}

vector  __vt_new(size_t count, size_t type_size) {
  if (!__vt_is_registered_atexit) {
    __vt_is_registered_atexit = 1;
    atexit(__vt_clean);
  }
  vector vt = (vector)calloc(1, sizeof(_vector));
  vt->__max_alloc_count = MAX_ALLOC_SIZE / type_size;
  vt->type_size = type_size;
  if (__vt_resize(vt, count)) {
    free(vt);
    return (void *)0;
  }
  __vt_vectors[__vt_vectors_count++] = vt;
  ++__vt_stack[__vt_stack_height];
  return vt;
}

void    __vt_delete(vector self) {
  while (self->arrays_count) {
    free(self->arrays[--self->arrays_count]);
  }
  free(self);
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

size_t  __vt_count(vector self) {
  return self->count;
}

int     __vt_empty(vector self) {
  return self->count == 0;
}

int     __vt_resize(vector self, size_t count) {
  if (self->count < count &&
      __vt_incre_limit(self, count - self->count)) {
    return -1;
  }
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
  if (__vt_incre_limit(self, 1)) {
    return -1;
  }
  ++self->count;
  __vt_post_incre(self);
  memcpy(__vt_at(self, self->count - 1), item, self->type_size);
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
  size_t arrays_idx = __vt_array_idx(self, idx);
  size_t prev_count = arrays_idx ? self->arrays_counts[arrays_idx - 1] : 0;
  return self->arrays[arrays_idx] + (idx - prev_count) * self->type_size;
}

void    __vt_move(vector dst, vector src) {
  memcpy(dst, src, sizeof(_vector));
  memset(src, '\0', sizeof(_vector));
}

void    __vt_copy(vector dst, vector src) {
  memcpy(dst, src, sizeof(_vector));
  for (size_t i = 0; i < dst->arrays_count; ++i) {
    size_t capacity = dst->arrays_counts[i] * dst->type_size;
    dst->arrays[i] = malloc(capacity);
    memcpy(dst->arrays[i], src->arrays[i], capacity);
  }
}
