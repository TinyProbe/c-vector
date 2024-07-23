#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "vector.h"

void test1(void) {
  vt_in();

  vector v = vt_new(0, char const *);
  vt_push(v, "hello, ");
  vt_push(v, "world!!!");
  vt_push(v, "af  sdifsj  asdi \n fow jowf  wioi  ");

  for (int i = 0; i < (int)vt_count(v); ++i) {
    printf("v[%d]: %s\n", i, vt_at(v, i, char const *));
  }

  vt_pop(v);
  vt_pop(v);

  for (int i = 0; i < (int)vt_count(v); ++i) {
    printf("v[%d]: %s\n", i, vt_at(v, i, char const *));
  }

  vt_clear(v);
  printf("%s\n", vt_empty(v) ? "empty" : "not empty");

  vt_out();
}

void test2(void) {
  vt_in();

  vector v = vt_new(1000, int);

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vt_deref(i, int) = i.idx + 1;
  }

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%d ", vt_deref(i, int));
  }
  printf("\n");

  vt_out();
}

void test3(void) {
  vt_in();

  vector v = vt_new(10, vector);

  for (int i = 0; i < (int)vt_size(v); ++i) {
    vector vv = vt_new(10, int);
    vt_at(v, i, vector) = vv;
    for (int j = 0; j < (int)vt_size(vv); ++j) {
      vt_at(vv, j, int) = i * 10 + j;
    }
  }

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vector vv = vt_deref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_size(vv); vt_next(j)) {
      printf("%d ", vt_deref(j, int));
    }
    printf("\n");
  }

  vt_out();
}

void test4(void) {
  vt_in();

  vector v = vt_new(0, long long);
  vt_push(v, 4023ll);
  vt_push(v, 290293202392ll);
  vt_push(v, 39202939393393920ll);
  while (!vt_empty(v)) {
    printf("%lld ", vt_back(v, long long));
    vt_pop(v);
  }
  printf("\n");

  vector v2 = vt_new(0, char);
  vt_push(v2, 'a');
  vt_push(v2, 'b');
  vt_push(v2, 'c');
  while (!vt_empty(v2)) {
    printf("%c ", vt_back(v2, char));
    vt_pop(v2);
  }
  printf("\n");

  vector v3 = vt_new(0, short);
  vt_push(v3, 4949);
  vt_push(v3, 30449);
  vt_push(v3, 1392);
  while (!vt_empty(v3)) {
    printf("%hd ", vt_back(v3, short));
    vt_pop(v3);
  }
  printf("\n");

  vt_out();
}

void test5(void) {
  vt_in();

  vector v = vt_new(100, int);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vt_deref(i, int) = i.idx + 1;
  }

  struct timeval t1, t2;
  gettimeofday(&t1, (void *)0);

  vt_reverse(v);

  gettimeofday(&t2, (void *)0);
  t2.tv_sec -= t1.tv_sec;
  t2.tv_usec -= t1.tv_usec;
  printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  // long long tot = 0;
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%d ", vt_deref(i, int));
    // tot += vt_deref(i, int);
  }
  printf("\n");
  // printf("%lld\n", tot);

  vt_out();
}

// void test6(void) {
//   vector v = vt_new(0, char);
//   for (int i = 0; "hello world!"[i]; ++i) {
//     vt_push(v, "hello world!"[i]);
//   }
//   for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
//     printf("%c", vt_deref(i, char));
//   }
//   printf("\n");
//   vt_delete(v);
// }

void test7(void) {
  vt_in();

  vector v = vt_new(100000, int);

  // Iterator (time take x4.*)
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   vt_deref(i, int) = ((i.idx + 1) * 5 + 10) * 5 % 10000;
  // }
  // long long tot = 0;
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   tot += vt_deref(i, int);
  // }

  // Random access (time take x9.*)
  for (size_t i = 0; i < vt_size(v); ++i) {
    vt_at(v, i, int) = ((i + 1) * 5 + 10) * 5 % 10000;
  }
  long long tot = 0;
  for (size_t i = 0; i < vt_size(v); ++i) {
    tot += vt_at(v, i, int);
  }

  printf("%lld\n", tot);

  vt_out();
}

void test7_1(void) {
  static size_t const _size = 100000;
  int a[_size];
  for (size_t i = 0; i < _size; ++i) {
    a[i] = ((i + 1) * 5 + 10) * 5 % 10000;
  }
  long long tot = 0;
  for (size_t i = 0; i < _size; ++i) {
    tot += a[i];
  }
  printf("%lld\n", tot);
}

void test8(void) {
  vt_in();

  vector v = vt_new(6, char const *);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vt_deref(i, char const *) = "hello World!!!";
  }
  vector v2 = vt_new(6, char const *);
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    vt_deref(i, char const *) = "!!!world hello";
  }
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%s\n", vt_deref(i, char const *));
  }
  printf("\n");
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    printf("%s\n", vt_deref(i, char const *));
  }
  printf("\n");
  vt_swap(v, v2);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%s\n", vt_deref(i, char const *));
  }
  printf("\n");
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    printf("%s\n", vt_deref(i, char const *));
  }
  printf("\n");

  vt_out();
}

typedef struct {
  int a;
  float b;
  long long c;
} Hello;

void test9(void) {
  vt_in();

  vector v = vt_new(10, Hello);
  for (int i = 0; i < 10; ++i) {
    Hello a = { 3, 5.5, 99999999999 };
    vt_itr itr = vt_iterator(v, i);
    vt_deref(itr, Hello) = a;
  }
  for (int i = 0; i < 10; ++i) {
    vt_itr itr = vt_iterator(v, i);
    Hello *a = &vt_deref(itr, Hello);
    printf("%d, %f, %lld\n", a->a, a->b, a->c);
  }

  vt_out();
}

int main(void) {
  // struct timeval t1, t2;
  // gettimeofday(&t1, (void *)0);

  // test1();
  // test2();
  // test3();
  // test4();
  // test5();
  // test6();
  // test7();
  // test7_1();
  // test8();
  test9();

  // gettimeofday(&t2, (void *)0);
  // t2.tv_sec -= t1.tv_sec;
  // t2.tv_usec -= t1.tv_usec;
  // printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  return 0;
}
