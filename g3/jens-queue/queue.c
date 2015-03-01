#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

void queue_init(queue_t *q) {
  // Using attr here makes it crash
  if (pthread_mutex_init(&q->head_mtx, NULL) != 0) {
    perror("mutex init failed");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_init(&q->tail_mtx, NULL) != 0) {
    perror("mutex init failed");
    exit(EXIT_FAILURE);
  }

  node_t *new = malloc(sizeof(node_t));
  if (new == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  new->next = NULL;
  q->head = q->tail = new;
}

void queue_put(queue_t *q, void *item) {
  /* lock the queue mutex */
  if (pthread_mutex_lock(&q->tail_mtx) != 0) {
    perror("mutex lock failed");
    exit(EXIT_FAILURE);
  }

  node_t *new = malloc(sizeof(node_t));
  if (new == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  new->item = item;
  new->next = NULL;

  /* add the new node to the tail */
  q->tail->next = new;
  q->tail = new;

  /* unlock the queue mutex */
  if (pthread_mutex_unlock(&q->tail_mtx) != 0) {
    perror("mutex unlock failed");
    exit(EXIT_FAILURE);
  }
}

void *queue_get(queue_t *q) {
  /* lock the queue mutex */
  if (pthread_mutex_lock(&q->head_mtx) != 0) {
    perror("mutex lock failed");
    exit(EXIT_FAILURE);
  }

  node_t *old = q->head;

  /* note that the head contains a 'dummy' node. That's why we test
   * old->next. */
  if (old->next == NULL) {
    if (pthread_mutex_unlock(&q->head_mtx) != 0) {
      perror("mutex unlock failed");
      exit(EXIT_FAILURE);
    }

    return NULL; /* queue was empty */
  }

  void *item = old->next->item;

  /* update the head and free the old memory */
  q->head = old->next;
  free(old);

  /* unlock the queue mutex */
  if (pthread_mutex_unlock(&q->head_mtx) != 0) {
    perror("mutex unlock failed");
    exit(EXIT_FAILURE);
  }

  return item;
}
