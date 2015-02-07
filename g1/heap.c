#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

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

// Build max heap
void heap_initialize(heap* h) {
  h->size = 0;
  h->root = NULL;
  h->alloc_size = 0;
}

void heap_clear(heap* h) {
  free(h->root);
  h->size = 0;
  h->alloc_size = 0;
}

size_t heap_size(heap* h) {
  return h->size;
}

void* heap_top(heap* h) {
  return h->root[0].value;
}

void heap_insert(heap* h, void* value, int priority) {
  h->size++;
  node n = {value, priority};
  h->alloc_size += sizeof(node);
  h->root = (node*)realloc(h->root, h->alloc_size);
  h->root[h->size - 1] = n;
  heap_max_heapify(h);
}

void* heap_pop(heap* h) {
  void* ptr = h->root[0].value;
  h->alloc_size -= sizeof(node);
  for (int i = 1; i < h->size; i++) {
    h->root[i - 1] = h->root[i];
  }
  h->root = (node*)realloc(h->root, h->alloc_size);
  h->size--;
  heap_max_heapify(h);
  return ptr;
}
