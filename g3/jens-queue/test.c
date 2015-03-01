#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

// Macro for printing name of function
// and then running it
#define run_test(fn_name)\
  printf("OK: %s\n", #fn_name);\
  fn_name();

static void *one = (void *)1;
static void *two = (void *)2;
static void *three = (void *)3;

void test_queue_basic_put_get() {
  queue_t q;
  queue_init(&q);

  queue_put(&q, one);
  queue_put(&q, two);
  queue_put(&q, three);

  assert(queue_get(&q) == one);
  assert(queue_get(&q) == two);
  assert(queue_get(&q) == three);
}

queue_t shared_q;

void* consumer(void *_) {
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10000; ++j);

    int *x = queue_get(&shared_q);
    if (x != NULL) {
      printf("consumer: %i\n", *x);
    }
  }
}

void* producer(void *_) {
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10000; ++j);

    int x = i + 1 - 1;
    queue_put(&shared_q, &x);
    printf("producer: %i\n", x);
  }
}

void test_queue_concurrently() {
  queue_init(&shared_q);
  pthread_t producer_thread;
  pthread_t consumer_thread;

  pthread_create(&producer_thread, NULL, &producer, NULL);
  pthread_create(&consumer_thread, NULL, &consumer, NULL);

  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);
}

int main(int argc, char **argv) {
  printf("\n");

  run_test(test_queue_basic_put_get);
  run_test(test_queue_concurrently);

  return 0;
}
