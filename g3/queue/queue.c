#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

struct Queue {
  void *head;
  Queue *tail;
};

Queue* queue_create() {
  Queue *q = malloc(sizeof(Queue));
  q->head = NULL;
  q->tail = NULL;
  return q;
}

size_t queue_size(Queue *q) {
  size_t result = 0;

  if (q->head == NULL) result = 0;
  else if (q->tail == NULL) result = 1;
  else {
    Queue *current = q;
    result++;
    while (current->tail != NULL) {
      result++;
      current = current->tail;
    }
  }

  return result;

  /* Recursive implementation */
  /* if (q->head == NULL) return 0; */
  /* if (q->tail == NULL) return 1; */
  /* return 1 + queue_size(q->tail); */
}

void queue_push(Queue *q, void *x) {
  pthread_mutex_lock(&mtx);
  if (q->head == NULL) {
    q->head = x;
  } else {
    Queue *current = q;
    while (current->tail != NULL) {
      current = current->tail;
    }
    Queue *p = queue_create();
    p->head = x;
    current->tail = p;
  }
  pthread_mutex_unlock(&mtx);

  /* Recursive implementation */
  /* if (q->head == NULL) { */
  /*   q->head = x; */
  /* } else if (q->tail == NULL) { */
  /*   Queue *p = queue_create(); */
  /*   p->head = x; */
  /*   q->tail = p; */
  /* } else { */
  /*   queue_push(q->tail, x); */
  /* } */
}

void* queue_pop(Queue *q) {
  assert(q->head != NULL);

  void *result = q->head;

  pthread_mutex_lock(&mtx);
  if (q->tail == NULL) {
    Queue *p = queue_create();
    *q = *(p);
  } else {
    *q = *(q->tail);
  }
  pthread_mutex_unlock(&mtx);

  return result;
}

int queue_empty(Queue *q) {
  return queue_size(q) == 0;
}
