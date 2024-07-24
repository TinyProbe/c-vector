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
    vt_ref(i, int) = i.idx + 1;
  }

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%d ", vt_ref(i, int));
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
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_size(vv); vt_next(j)) {
      printf("%d ", vt_ref(j, int));
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

typedef struct {
  int a;
  double b;
  long long c;
  char d[(1 << 8) + 13];
} T1;

void test5(void) {
  vt_in();

  // vector v = vt_new(100000, long long);
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   vt_ref(i, long long) = ((i.idx * 44 + 11) * 88 + 33) % 100000;
  // }
  vector v = vt_new(100000, T1);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    T1 *p = &vt_ref(i, T1);
    p->a = ((i.idx * 55 + 33) * 99 + 77) % 55555;
    p->b = i.idx * 1.5 + 11;
    p->c = i.idx * 130985 + 4590;
    for (int j = 0; j < (1<<8)+13; ++j) {
      p->d[j] = i.idx % 10 + '0';
    }
  }

  struct timeval t1, t2;
  gettimeofday(&t1, nullptr);

  vt_reverse(v);

  gettimeofday(&t2, nullptr);
  t2.tv_sec -= t1.tv_sec;
  t2.tv_usec -= t1.tv_usec;
  printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  // long long tot = 0;
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   // printf("%d ", vt_ref(i, T1).a);
  //   tot += vt_ref(i, long long);
  // }
  // // printf("\n");
  // printf("%lld\n", tot);
  long long tot = 0;
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    // printf("%d ", vt_ref(i, T1).a);
    tot += vt_ref(i, T1).a;
  }
  // printf("\n");
  printf("%lld\n", tot);

  vt_out();
}

// void test6(void) {
//   vector v = vt_new(0, char);
//   for (int i = 0; "hello world!"[i]; ++i) {
//     vt_push(v, "hello world!"[i]);
//   }
//   for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
//     printf("%c", vt_ref(i, char));
//   }
//   printf("\n");
//   vt_delete(v);
// }

void test7(void) {
  vt_in();

  vector v = vt_new(100000, int);

  // Iterator (time take x4.*)
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   vt_ref(i, int) = ((i.idx + 1) * 5 + 10) * 5 % 10000;
  // }
  // long long tot = 0;
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   tot += vt_ref(i, int);
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
    vt_ref(i, char const *) = "hello World!!!";
  }
  vector v2 = vt_new(6, char const *);
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    vt_ref(i, char const *) = "!!!world hello";
  }

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%s\n", vt_ref(i, char const *));
  }
  printf("\n");
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    printf("%s\n", vt_ref(i, char const *));
  }
  printf("\n");

  vt_swap(v, v2);

  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    printf("%s\n", vt_ref(i, char const *));
  }
  printf("\n");
  for (vt_itr i = vt_begin(v2); i.idx < vt_size(v2); vt_next(i)) {
    printf("%s\n", vt_ref(i, char const *));
  }
  printf("\n");

  vt_out();
}

typedef struct {
  int a;
  double b;
  long long c;
} Hello;

void test9(void) {
  vt_in();

  vector v = vt_new(10, Hello);
  Hello a = { 3, 5.5, 99999999999 };
  for (int i = 0; i < 10; ++i) {
    vt_itr itr = vt_itr(v, i);
    vt_ref(itr, Hello) = a;
    a.a += a.a;
    a.b += a.b;
  }
  for (int i = 0; i < 10; ++i) {
    vt_itr itr = vt_itr(v, i);
    Hello *a = &vt_ref(itr, Hello);
    printf("%d, %f, %lld\n", a->a, a->b, a->c);
  }

  vt_out();
}

void test10(void) {
  vt_in();

  vector v = vt_new(10, vector);
  Hello a = { 2, 1.47, 29 };
  for (vt_itr i = vt_begin(v); i.idx < vt_len(v); vt_next(i)) {
    vector vv = vt_ref(i, vector) = vt_new(10, Hello);
    for (vt_itr j = vt_begin(vv); j.idx < vt_len(vv); vt_next(j)) {
      vt_ref(j, Hello) = a;
      a.a += a.a;
      a.b += a.b;
      a.c += a.c;
    }
  }
  for (vt_itr i = vt_begin(v); i.idx < vt_len(v); vt_next(i)) {
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_len(vv); vt_next(j)) {
      Hello *b = &vt_ref(j, Hello);
      printf("%d, %f, %lld\n", b->a, b->b, b->c);
    }
    printf("\n");
  }

  vector v2 = vt_clone(v, 2);
  for (vt_itr i = vt_begin(v2); i.idx < vt_len(v2); vt_next(i)) {
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_len(vv); vt_next(j)) {
      Hello *b = &vt_ref(j, Hello);
      printf("%d, %f, %lld\n", b->a, b->b, b->c);
    }
    printf("\n");
  }

  vt_out();
}

int main(void) {
  // struct timeval t1, t2;
  // gettimeofday(&t1, nullptr);

  // test1();
  // test2();
  // test3();
  // test4();
  // test5();
  // test6();
  // test7();
  // test7_1();
  // test8();
  // test9();
  // test10();

  // gettimeofday(&t2, nullptr);
  // t2.tv_sec -= t1.tv_sec;
  // t2.tv_usec -= t1.tv_usec;
  // printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  return 0;
}
