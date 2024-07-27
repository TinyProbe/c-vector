#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "vector.h"

void test1(void) {
  vt_in();

  vector v = vt_new(char const *, 1, 0);
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

  vector v = vt_new(int, 1, 100000);

  for (size_t i = 0; i < vt_len(v); ++i) {
    vt_at(v, i, int) = i + 1;
  }

  long long tot = 0;
  for (size_t i = 0; i < vt_len(v); ++i) {
    tot += vt_at(v, i, int);
  }
  printf("%lld\n", tot);

  vt_out();
}

void test3(void) {
  vt_in();

  vector v = vt_new(int, 2, 1000, 1000);

  for (int i = 0; i < (int)vt_size(v); ++i) {
    vector vv = vt_at(v, i, vector);
    for (int j = 0; j < (int)vt_size(vv); ++j) {
      vt_at(vv, j, int) = i * 10 + j;
    }
  }

  long long tot = 0;
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_size(vv); vt_next(j)) {
      // printf("%d ", vt_ref(j, int));
      tot += vt_ref(j, int);
    }
    // printf("\n");
  }
  printf("%lld\n", tot);

  vt_out();
}

void test4(void) {
  vt_in();

  vector v = vt_new(long long, 1, 0);
  vt_push(v, 4023ll);
  vt_push(v, 290293202392ll);
  vt_push(v, 39202939393393920ll);
  while (!vt_empty(v)) {
    printf("%lld ", vt_back(v, long long));
    vt_pop(v);
  }
  printf("\n");

  vector v2 = vt_new(char, 1, 0);
  vt_push(v2, 'a');
  vt_push(v2, 'b');
  vt_push(v2, 'c');
  while (!vt_empty(v2)) {
    printf("%c ", vt_back(v2, char));
    vt_pop(v2);
  }
  printf("\n");

  vector v3 = vt_new(short, 1, 0);
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

  vector v = vt_new(long long, 1, 100000);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vt_ref(i, long long) = ((i.idx * 44 + 11) * 88 + 33) % 100000;
  }
  // vector v = vt_new(T1, 1, 100000);
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   T1 *p = &vt_ref(i, T1);
  //   p->a = ((i.idx * 55 + 33) * 99 + 77) % 55555;
  //   p->b = i.idx * 1.5 + 11;
  //   p->c = i.idx * 130985 + 4590;
  //   for (int j = 0; j < (1<<8)+13; ++j) {
  //     p->d[j] = i.idx % 10 + '0';
  //   }
  // }

  struct timeval t1, t2;
  gettimeofday(&t1, __nullptr);

  vt_reverse(v);

  gettimeofday(&t2, __nullptr);
  t2.tv_sec -= t1.tv_sec;
  t2.tv_usec -= t1.tv_usec;
  printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  long long tot = 0;
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    // printf("%d ", vt_ref(i, T1).a);
    tot += vt_ref(i, long long);
  }
  // printf("\n");
  printf("%lld\n", tot);
  // long long tot = 0;
  // for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
  //   // printf("%d ", vt_ref(i, T1).a);
  //   tot += vt_ref(i, T1).a;
  // }
  // // printf("\n");
  // printf("%lld\n", tot);

  vt_out();
}

// void test6(void) {
//   vector v = vt_new(char, 1, 0);
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

  vector v = vt_new(int, 1, 1000000);

  // Iterator (time take x1.*)
  // long long tot = 0;
  // for (vt_itr i = vt_begin(v); i.idx < vt_len(v); vt_next(i)) {
  //   tot += vt_ref(i, int) = ((i.idx + 1) * 5 + 10) * 5 % 10000;
  // }

  // Random access (same as array)
  long long tot = 0;
  for (size_t i = 0; i < vt_size(v); ++i) {
    tot += vt_at(v, i, int) = ((i + 1) * 5 + 10) * 5 % 10000;
  }

  printf("%lld\n", tot);

  vt_out();
}

void test7_1(void) {
  static size_t const _size = 1000000;
  int a[_size];
  long long tot = 0;
  for (size_t i = 0; i < _size; ++i) {
    tot += a[i] = ((i + 1) * 5 + 10) * 5 % 10000;
  }
  printf("%lld\n", tot);
}

void test8(void) {
  vt_in();

  vector v = vt_new(char const *, 1, 6);
  for (vt_itr i = vt_begin(v); i.idx < vt_size(v); vt_next(i)) {
    vt_ref(i, char const *) = "hello World!!!";
  }
  vector v2 = vt_new(char const *, 1, 6);
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

  vector v = vt_new(Hello, 1, 10);
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

  vector v = vt_new(Hello, 2, 300, 300);
  Hello a = { 2, 1.47, 29 };
  for (vt_itr i = vt_begin(v); i.idx < vt_len(v); vt_next(i)) {
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_len(vv); vt_next(j)) {
      vt_ref(j, Hello) = a;
      a.a += 7;
      a.b += 4.83;
      a.c += 43;
    }
  }

  struct timeval t1, t2;
  gettimeofday(&t1, __nullptr);

  vector v2 = vt_clone(v);

  gettimeofday(&t2, __nullptr);
  t2.tv_sec -= t1.tv_sec;
  t2.tv_usec -= t1.tv_usec;
  printf("%lds %ldus\n", t2.tv_sec, t2.tv_usec);

  long long tot = 0;
  for (vt_itr i = vt_begin(v2); i.idx < vt_len(v2); vt_next(i)) {
    vector vv = vt_ref(i, vector);
    for (vt_itr j = vt_begin(vv); j.idx < vt_len(vv); vt_next(j)) {
      tot += vt_ref(j, Hello).c;
    }
  }
  printf("%lld\n", tot);

  vt_out();
}

void test11(void) {
  vt_in();

  // vector v = vt_new(int, 1, 0);
  // for (int i = 0; i < 1000000; ++i) {
  //   vt_push(v, ((i + 1) * 5 + 17) * 3);
  // }
  // long long tot = 0;
  // while (vt_len(v)) {
  //   tot += vt_back(v, int);
  //   vt_pop(v);
  // }
  // printf("%lld\n", tot);

  int a[1000000];
  for (int i = 0; i < 1000000; ++i) {
    a[i] = ((i + 1) * 5 + 17) * 3;
  }
  long long tot = 0;
  for (int i = 1000000; i--; ) {
    tot += a[i];
  }
  printf("%lld\n", tot);

  vt_out();
}

void test12(void) {
  vt_in();

  long long tot = 0;

  // vector v = vt_new(int, 1, 500000);
  // for (vt_itr i = vt_begin(v); i.idx < vt_len(v); vt_next(i)) {
  //   tot += vt_ref(i, int) = ((i.idx + 19) * 71 + 13) * 9;
  // }

  // vector v = vt_new(int, 1, 500000);
  // for (size_t i = 0; i < vt_len(v); ++i) {
  //   tot += vt_at(v, i, int) = ((i + 19) * 71 + 13) * 9;
  // }

  int a[500000];
  for (int i = 0; i < 500000; ++i) {
    tot += a[i] = ((i + 19) * 71 + 13) * 9;
  }
  printf("%d\n", a[99993]);

  printf("%lld\n", tot);

  vt_out();
}

void test13(void) {
  vt_in();

  vector v = vt_new(int, 1, 0);
  for (int i = 0; i < 1000000; ++i) {
    vt_push(v, ((i + 1) * 5 + 13) * 15);
  }
  // vector v = vt_new(int, 1, 1000000);
  // for (size_t i = 0; i < vt_len(v); ++i) {
  //   vt_at(v, i, int) = ((i + 1) * 5 + 13) * 15;
  // }

  long long tot = 0;
  for (size_t i = 0; i < vt_len(v); ++i) {
    tot += vt_at(v, i, int);
  }
  printf("%lld\n", tot);

  // size_t const SIZE = 1000000;
  // int a[SIZE];
  // for (int i = 0; i < (int)SIZE; ++i) {
  //   a[i] = ((i + 1) * 5 + 13) * 15;
  // }

  // long long tot = 0;
  // for (int i = 0; i < (int)SIZE; ++i) {
  //   tot += a[i];
  // }
  // printf("%lld\n", tot);

  vt_out();
}

void test14(void) {
  vt_in();

  vector v = vt_new(int, 1, 0);
  for (int i = 0; i < 100000; ++i) {
    vt_push(v, ((i + 1) * 5 + 13) * 15);
  }
  for (int i = 0; i < 50000; ++i) {
    vt_pop(v);
  }
  for (int i = 0; i < 30000; ++i) {
    vt_push(v, ((i + 1) * 5 + 13) * 15);
  }
  for (int i = 0; i < 40000; ++i) {
    vt_pop(v);
  }

  long long tot = 0;
  for (size_t i = 0; i < vt_len(v); ++i) {
    tot += vt_at(v, i, int);
  }
  printf("%lld\n", tot);

  vt_out();
}

void test15(void) {
  vt_in();

  vector v = vt_new(int, 1, 0);
  for (int i = 0; i < 100000; ++i) {
    vt_input(v, i, i + 1);
  }
  for (int i = 0; i < 9000; ++i) {
    vt_erase(v, 90000);
  }

  long long tot = 0;
  for (size_t i = 0; i < vt_len(v); ++i) {
    // printf("%d ", vt_at(v, i, int));
    tot += vt_at(v, i, int);
  }
  // printf("\n");
  printf("%lld\n", tot);

  vt_out();
}

int main(void) {
  struct timeval t1, t2;
  gettimeofday(&t1, __nullptr);

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
  // test11();
  // test12();
  // test13();
  // test14();
  // test15();

  gettimeofday(&t2, __nullptr);
  t2.tv_sec -= t1.tv_sec;
  t2.tv_usec -= t1.tv_usec;
  printf("%lus %luus\n", t2.tv_sec, t2.tv_usec);

  return 0;
}
