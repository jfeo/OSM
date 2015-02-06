#include <stddef.h> // size_t

typedef struct {
  void* value;
  int priority;
} node;

typedef struct {
  node* root;
  size_t size;
  /* You may want to allocate more
     space than strictly needed. */
  size_t alloc_size;
} heap;

int heap_parent(int);
int heap_left(int);
int heap_right(int);
void heap_max_heapify(heap*);

void heap_initialize(heap*);
void heap_clear(heap*);
size_t heap_size(heap*);
void* heap_top(heap*);
void heap_insert(heap*, void*, int);
void *heap_pop(heap*);
