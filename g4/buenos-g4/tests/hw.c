/*
 * Userland helloworld
 */

#include "tests/lib.h"

static const size_t BUFFER_SIZE = 20;

int main(void) {
  heap_init();

  int new_end = 1024 * 17;
  void *x = syscall_memlimit((void *)new_end);
  printf("pointer: %p\n", x);

  return 0;
}
