#include "heap.h"
#include <stdio.h>

int main(void) {
  heap h;
  heap_initialize(&h);
  int v1 = 10;
  heap_insert(&h, &v1, 10);
  int v2 = 5;
  heap_insert(&h, &v2, 5);
  int v3 = 100;
  heap_insert(&h, &v3, 100);
  int v4 = 60;
  heap_insert(&h, &v4, 60);
  int v5 = 70;
  heap_insert(&h, &v5, 70); 
  for (int i = 1; i <= 5; i++) {
    printf("Top value is: %d.\n", *((int*)heap_top(&h)));
    printf("Value pop number %d is: %d.\n", i, *((int*)heap_pop(&h)));
  }
  heap_clear(&h);
  printf("Size is: %d.\n", (int)heap_size(&h));
}
