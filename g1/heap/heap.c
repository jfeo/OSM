#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

#define INIT_SIZE 100 // initial capacity of the heap

size_t heap_leftchild(heap* h, size_t i) {
  assert(h->size > 1 && i < h->size);
  return i*2+1;
}

size_t heap_rightchild(heap* h, size_t i) {
  assert(h->size > 2 && i < h->size);
  return i*2+2;
}

size_t heap_parent(heap* h, size_t i) {
  assert(i < h->size);
  if(h->size < 2) {
    return 0;
  }
  return (i-1)/2;
}

void heap_clear(heap* h) {
  free(h->root);
  heap_initialize(h);
}

void heap_heapify(heap* h, size_t i) {
  node* parent = &h->root[i];

  size_t largest = i;

  if(h->size > 1) {
    size_t left = heap_leftchild(h, i);
    if(left <= h->size && h->root[left].priority > parent->priority) {
      largest = left;
    }
    if(h->size > 2 ) {
      size_t right = heap_rightchild(h, i);
      if(right <= h->size && h->root[right].priority > h->root[largest].priority) {
        largest = right;
      }
    }
  }
  if(largest != i) {
    // swap
    node tmp = *parent;
    *parent = h->root[largest];
    h->root[largest] = tmp;
    heap_heapify(h, largest);
  }
}

void heap_initialize(heap* h) {
  size_t init_bytes = INIT_SIZE * sizeof(node);
  h->size = 0;
  h->alloc_size = INIT_SIZE;
  h->root = malloc(init_bytes);
}

size_t heap_size(heap* h) {
  return (*h).size;
}

void heap_insert(heap* h, void* data, int prio) {
  assert(prio > 0); // we don't allow for negative priorities
  h->size += 1;
  if(h->size > h->alloc_size) {
    // expand and realloc
    h->alloc_size = h->alloc_size * 2; // double our space
    int realloc_bytes = h->alloc_size * sizeof(node);
    h->root = realloc(h->root, realloc_bytes);
  }
  // insert at end
  node* end = h->root + h->size - 1;
  end->value = data;
  end->priority = prio;
  // re-sort
  for (size_t i = (h->size/2); i > 0 ; i--) {
    heap_heapify(h, i-1);
  }
}

void* heap_pop(heap* h) {
  if(h->size == 0) return NULL;
  void* value = h->root->value;
  // pop the top down to bottom (end of array)
  h->root->priority = 0;
  heap_heapify(h, 0);
  h->size--; 
  return value;
}

void* heap_top(heap* h) {
  if(heap_size(h) == 0) return NULL;
  return h->root->value;
}
