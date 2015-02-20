#include <stdlib.h>

typedef struct Queue Queue;

Queue* queue_create();
size_t queue_size(Queue *q);
void   queue_push(Queue *q, void *x);
void*  queue_pop(Queue *q);
int    queue_empty(Queue *q);
