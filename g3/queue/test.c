#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "queue.h"

#define run_test(fn_name)\
  printf("OK: %s\n", #fn_name);\
  fn_name();

// Dummy pointers used for testing
static void *ONE = (void *)1;
static void *TWO = (void *)2;
static void *THREE = (void *)3;

void test_creating_empty_queue() {
  Queue *q = queue_create();
  assert(0 == queue_size(q));
}

void test_push() {
  Queue *q = queue_create();

  queue_push(q, ONE);
  queue_push(q, TWO);
  queue_push(q, THREE);

  assert(3 == queue_size(q));
}

void test_pop() {
  Queue *q = queue_create();
  queue_push(q, ONE);

  assert(ONE == queue_pop(q));
}

void test_push_and_pop_more_things() {
  Queue *q = queue_create();
  queue_push(q, ONE);
  queue_push(q, TWO);
  queue_push(q, THREE);

  assert(3 == queue_size(q));
  assert(ONE == queue_pop(q));

  assert(2 == queue_size(q));
  assert(TWO == queue_pop(q));

  assert(1 == queue_size(q));
  assert(THREE == queue_pop(q));

  assert(0 == queue_size(q));
}

void test_queue_empty() {
  Queue *q = queue_create();
  assert(queue_empty(q));
  queue_push(q, ONE);
  assert(!queue_empty(q));
}

static Queue *shared_queue;
static void *results[3];

void *push() {
  queue_push(shared_queue, ONE);
  queue_push(shared_queue, TWO);
  queue_push(shared_queue, THREE);

  pthread_exit(NULL);
}

void *pop() {
  for (int i = 0; i < 3; ++i) {
    while (queue_empty(shared_queue));
    results[i] = queue_pop(shared_queue);
  }

  pthread_exit(NULL);
}

void test_concurrent_access() {
  shared_queue = queue_create();

  pthread_t pusher;
  pthread_t poper;
  pthread_create(&pusher, NULL, push, NULL);
  pthread_create(&poper, NULL, pop, NULL);
  pthread_join(pusher, NULL);
  pthread_join(poper, NULL);

  assert(ONE == results[0]);
  assert(TWO == results[1]);
  assert(THREE == results[2]);
}

int main(int argc, char *argv[]) {
  printf("\n");

  run_test(test_creating_empty_queue);
  run_test(test_push);
  run_test(test_pop);
  run_test(test_push_and_pop_more_things);
  run_test(test_queue_empty);
  run_test(test_concurrent_access);

  return 0;
}
