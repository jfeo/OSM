#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

// Return index of parent of node i
int heap_parent(int i);
// Return index of left child of node i
int heap_left(int i);
// Return index of right child of node i
int heap_right(int i);
// Preserve max heap property of heap
void heap_max_heapify(heap* h);

// Initalize heap 
void heap_initialize(heap* h) {
  h->size = 0;
  h->root = NULL;
  h->alloc_size = 0;
}

// Free memory and reset size variables
void heap_clear(heap* h) {
  free(h->root);
  h->size = 0;
  h->alloc_size = 0;
}


// Return the number of nodes in the heap
size_t heap_size(heap* h) {
  return h->size;
}

// Return the top (maximal priority) value
void* heap_top(heap* h) {
  if (heap_size(h) > 0) {
    return h->root[0].value;
  } else {
    return NULL;
  }
}

// Insert value with priority into the heap
void heap_insert(heap* h, void* value, int priority) {
  node n = {value, priority};
  h->size++;
  h->alloc_size += sizeof(node);
  h->root = (node*)realloc(h->root, h->alloc_size);
  h->root[h->size - 1] = n;
  heap_max_heapify(h);
}

// Pop top node off the heap and return value
void* heap_pop(heap* h) {
  if (h->size > 0) {
    void* ptr = h->root[0].value;
    h->alloc_size -= sizeof(node);
    for (int i = 1; i < h->size; i++) {
      h->root[i - 1] = h->root[i];
    }
    h->root = (node*)realloc(h->root, h->alloc_size);
    h->size--;
    heap_max_heapify(h);
    return ptr;
  } else {
    return NULL;
  }
}

// Helper methods

int heap_parent(int i) {
  return i / 2;
}

int heap_left(int i) {
  return 2 * i;
}

int heap_right(int i) {
  return 2 * i + 1;
}

void heap_max_heapify(heap* h) {
  for (int i = h->size - 1; i >= 0; --i) {
    if (heap_left(i) < h->size && h->root[heap_left(i)].priority > h->root[i].priority) {
      node highest = h->root[heap_left(i)];
      node lowest = h->root[i];
      h->root[i] = highest;
      h->root[heap_left(i)] = lowest;
    }
    if (heap_right(i) < h->size && h->root[heap_right(i)].priority > h->root[i].priority) {
      node highest = h->root[heap_right(i)];
      node lowest = h->root[i];
      h->root[i] = highest;
      h->root[heap_right(i)] = lowest;
    } 
  }
}

