#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

char *success(int res) {
  if (res == 0)
    return "Failure!";
  else
    return "Success!";
}

// Macro for printing name of function
// and then running it
#define run_test(fn_name)\
  printf("Testing: %s. %s\n", #fn_name, success(fn_name()));\

int test_queue_put(void) {
  queue_t q;
  
  queue_init(&q);

  int vals[] = {42, 24, 12};

  queue_put(&q, &vals[0]);
  queue_put(&q, &vals[1]);
  queue_put(&q, &vals[2]);
  
  if (*(int*)q.head->next->item != 42)
    return 0;
  if (*(int*)q.head->next->next->item != 24)
    return 0;
  if (*(int*)q.head->next->next->next->item != 12)
    return 0;    

  return 1;
}

int main(int argc, char **argv) {
  run_test(test_queue_put);

	return 0;
}
