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

// Initalize heap 
void heap_initialize(heap*);
// Free memory and reset size variables
void heap_clear(heap*);
// Return the number of nodes in the heap
size_t heap_size(heap*);
// Return the top (maximal priority) value
void* heap_top(heap*);
// Insert value with priority into the heap
void heap_insert(heap*, void*, int);
// Pop top node off the heap and return value
void *heap_pop(heap*);
