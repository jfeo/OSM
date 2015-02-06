#include "heap.h"
#include <stdio.h>

int main(void) {
  heap h;
  heap_initialize(&h);
  int v1 = 10;
  int v2 = 5;
  heap_insert(&h, &v1, 324321);
  heap_insert(&h, &v2, 10);
  printf("lol er %d", *((int*)heap_pop(&h)));
  printf("lol er %d", *((int*)heap_pop(&h)));
}
