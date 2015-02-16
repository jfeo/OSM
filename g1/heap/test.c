#include "heap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Macro for printing name of function
// and then running it
#define run_test(fn_name)\
  printf("OK: %s\n", #fn_name);\
  fn_name();

// Dummy pointers for testing
static void *ONE = (void *)1;
static void *TWO = (void *)2;
static void *THREE = (void *)3;

// Helper function to create heap and initialize it
heap* make_heap() {
  heap *h = (heap *)malloc(sizeof(heap));
  assert(h != NULL);
  heap_initialize(h);
  return h;
}

void test_heap_initialize() {
  heap *h  = make_heap();

  assert(heap_size(h) == 0);
}

void test_heap_insert() {
  heap *h = make_heap();

  heap_insert(h, ONE, 10);

  assert(heap_size(h) == 1);
}

void test_heap_top() {
  heap *h = make_heap();

  heap_insert(h, TWO, 2);
  heap_insert(h, THREE, 3);
  heap_insert(h, ONE, 1);

  assert(heap_top(h) == THREE);
}

void test_heap_top_empty() {
  heap *h = make_heap();

  assert(heap_top(h) == NULL);
}

void test_heap_pop() {
  heap *h = make_heap();
  heap_insert(h, ONE, 1);
  heap_insert(h, TWO, 2);
  heap_insert(h, THREE, 3);

  void *x = heap_pop(h);
  assert(heap_size(h) == 2);
  assert(x == THREE);

  void *y = heap_pop(h);
  assert(heap_size(h) == 1);
  assert(y == TWO);

  void *z = heap_pop(h);
  assert(heap_size(h) == 0);
  assert(z == ONE);
}

void test_heap_pop_empty() {
  heap *h = make_heap();

  void *x = heap_pop(h);

  assert(x == NULL);
}

void test_heap_clear() {
  heap *h = make_heap();
  heap_insert(h, TWO, 2);
  heap_insert(h, THREE, 3);
  heap_insert(h, ONE, 1);

  heap_clear(h);

  assert(heap_size(h) == 0);
  assert(heap_top(h) == NULL);

  heap_insert(h, ONE, 10);
  assert(heap_top(h) == ONE);
}

void test_heap_many() {
  heap h;
  
  heap_initialize(&h);
  int count = 2000;
  for (int i = 1; i < count; ++i) {
    heap_insert(&h, ONE, i);
  }
  assert(heap_size(&h) == count-1);
}

int main(int argc, char *argv[]) {
  printf("\n");

  run_test(test_heap_initialize);
  run_test(test_heap_insert);
  run_test(test_heap_top);
  run_test(test_heap_top_empty);
  run_test(test_heap_pop);
  run_test(test_heap_pop_empty);
  run_test(test_heap_clear);
  run_test(test_heap_many);

  return 0;
}
