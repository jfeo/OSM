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

/* int main2(void) */
/* { */
/*   char *name; */
/*   int count; */
/*   heap_init(); */
/*   puts("Hello, World!\n\n"); */
/*   while (1) { */
/*     name = (char*)malloc(BUFFER_SIZE); */
/*     printf("Please enter your name (max %d chars): ", BUFFER_SIZE); */
/*     count = readline(name, BUFFER_SIZE); */
/*     if (count == 0) { */
/*       break; */
/*     } */
/*     name[count] = 0; /1* Chomp off newline *1/ */
/*     printf("And hello to you, %s!\n", name); */
/*     free(name); */
/*   } */
/*   puts("Now I shall exit!\n"); */
/*   syscall_exit(2); */
/*   return 0; */
/* } */
