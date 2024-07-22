#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vector.h"

void test1() {
  vt_begin();

  vector v = vt_new(0, char const *);
  vt_push(v, "hello, ");
  vt_push(v, "world!!!");
  vt_push(v, "asdfasdif j092j 902j 02f");

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

  vt_end();
}

void test2() {
  vt_begin();

  vector v = vt_new(1000, int);
  for (int i = 0; i < (int)vt_size(v); ++i) {
    vt_at(v, i, int) = i + 1;
  }
  for (int i = 0; i < (int)vt_size(v); ++i) {
    printf("%d ", vt_at(v, i, int));
  }
  printf("\n");

  vt_end();
}

void test3() {
  vt_begin();

  vector v = vt_new(10, vector);
  for (int i = 0; i < (int)vt_size(v); ++i) {
    vector vv = vt_new(10, int);
    vt_at(v, i, vector) = vv;
    for (int j = 0; j < (int)vt_size(vv); ++j) {
      vt_at(vv, j, int) = i * 10 + j;
    }
  }
  for (int i = 0; i < (int)vt_size(v); ++i) {
    vector vv = vt_at(v, i, vector);
    for (int j = 0; j < (int)vt_size(vv); ++j) {
      printf("%d%c", vt_at(vv, j, int),
             j < (int)vt_size(vv) - 1 ? ' ' : '\n');
    }
  }

  vt_end();
}

void test4() {
  vt_begin();

  vector v = vt_new(0, long long);
  vt_push(v, 145ll);
  vt_push(v, 492ll);
  vt_push(v, 4023ll);
  vt_push(v, 290293202392ll);
  vt_push(v, 3920293939339ll);
  while (!vt_empty(v)) {
    printf("%lld ", vt_back(v, long long));
    vt_pop(v);
  }
  printf("\n");

  vector v2 = vt_new(0, char);
  vt_push(v2, 'a');
  vt_push(v2, 'b');
  vt_push(v2, 'c');
  vt_push(v2, 'd');
  vt_push(v2, 'e');
  while (!vt_empty(v2)) {
    printf("%c ", vt_back(v2, char));
    vt_pop(v2);
  }
  printf("\n");

  vector v3 = vt_new(0, short);
  vt_push(v3, 199);
  vt_push(v3, 4949);
  vt_push(v3, 30449);
  vt_push(v3, 1392);
  vt_push(v3, 2902);
  while (!vt_empty(v3)) {
    printf("%hd ", vt_back(v3, short));
    vt_pop(v3);
  }
  printf("\n");

  vt_end();
}

int main(void) {
  // test1();
  // test2();
  // test3();
  // test4();

  return 0;
}
