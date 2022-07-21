#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "../Defs.h"

// macros used in heap
#define LEFTCHILD(x) 2 * x + 1
#define RIGHTCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

typedef struct Node
{
    processData *data;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *front;
    Node *rear;
    int size;
} Queue;

/// PRIORITY QUEUE
typedef struct PriorityQueue
{
    processData *elements;
    ALGORITHM algo;
    int size;
} PriorityQueue;

//==================================QUEUE==========================================

Queue* Queue_init(); // initialize the queue
void enqueue(Queue *Q, processData *data);
processData* dequeue(Queue *q);
processData *front(Queue *q);
int isEmpty(Queue *q);
int size_queue(Queue *q);

//==================================PRIORITY QUEUE=================================

PriorityQueue* PriorityQueue_init(ALGORITHM algo); // initialize the Priority queue
void push(PriorityQueue *hp, processData *data);
processData *pop(PriorityQueue *hp);
processData *peek(PriorityQueue *hp);
int Empty(PriorityQueue *hp);
int size_prioQ(PriorityQueue *hp);

//=================================================================================
#endif
