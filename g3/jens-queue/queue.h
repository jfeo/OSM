#include <pthread.h>

typedef struct node {
  void *item;
  struct node *next;
} node_t;

typedef struct queue {
  pthread_mutex_t head_mtx;
  pthread_mutex_t tail_mtx;
  pthread_cond_t cond;
  pthread_mutexattr_t attr;
  node_t *head;
  node_t *tail;
} queue_t;

/* queue_init initializes a new queue */
void queue_init(queue_t *q);

/* queue_put adds the item to the tail of the queue */
void queue_put(queue_t *q, void *item);

/* queue_get removes and returns the head of the queue in item */
void *queue_get(queue_t *q);
