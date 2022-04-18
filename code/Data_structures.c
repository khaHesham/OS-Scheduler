#include <stdio.h>
#include <stdlib.h>

typedef short bool;
#define true 1
#define false 0
#define SHKEY 300
//////new
#define LEFTCHILD(x) 2 * x + 1  // macros used in heap
#define RIGHTCHILD(x) 2 * x + 2 // x here is index in the heap array
#define PARENT(x) (x - 1) / 2
/*
        MinHeap  --> Remember:parent is always less than his children
                     indecies:                 0 1  2 3  4  5  6
            4        array implementation: -->  4 8 10 12 11 17 19    #leftchild(index 0)=2*0+1=[1]=8   #rightchild(index 2)=2*2+2= [6]=19
          8   10
       12 11 17 19
*/

enum algorithm // used to decide which priority we will take in the priority queue
{
    SRTN, // remaining time is the periority
    HPF,  // periority
};

// used in priority queue
typedef struct ProcessData // typedef here means  that i can create a struct using only the name of it without typing struct before it
{                          // ex:  -struct ProcessData data  (Wrong || old way implementation)  -ProcessData data (correct) //simple isn't it? :D
    int remainingTime;
    int ID;
    int priority;
    /*
    int arrivaltime;  <<<<we may need to store them also>>>>
    int runningtime;
    */

} ProcessData;

/*
               / ******************************************************************* \
            ************ ========> (((# QUEUE & PRIORITY QUEUE #)))) <========= ************
               \ ******************************************************************* /
*/

///  QUEUE
typedef struct Node
{
    struct ProcessData *data;
    struct Node *next;
} Node;

typedef struct Queue
{
    struct Node *front;
    struct Node *rear;
    int size;
} Queue;

Node *newNode(ProcessData data)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = &data;
    temp->next = NULL;
    return temp;
}

Queue init() // initialize the queue
{
    Queue q;
    q.front = q.rear = NULL;
    q.size = 0;
    return q;
}

void dequeue(Queue *q)
{
    if (q->size == 0)
        return;
    Node *temp = q->front;
    q->front = q->front->next;
    free(temp); // built in function used to free unused allocated memory!
    if (q->front == NULL)
        q->rear = NULL;
    q->size -= 1;
}

ProcessData *front(Queue *q)
{
    if (q->front != NULL)
        return q->front->data;
}
ProcessData *rear(Queue *q)
{
    if (q->rear != NULL)
        return q->rear->data;
}
void enqueue(Queue *Q, ProcessData data)
{
    Node *temp = newNode(data);
    if (Q->rear == NULL) // 5 enqueu ->
    {
        Q->front = Q->rear = temp;
        Q->size += 1;
        return;
    } //  5 enqueu ->      f-> 1 2 3 4 <-r      --->  f-> 1 2 3 4 5 <-r
    Q->rear->next = temp;
    Q->rear = temp;
    Q->size += 1;
    return;
}
int isEmpty(Queue *q)
{
    return q->size == 0;
}

/// PRIORITY QUEUE
typedef struct MinHeap
{
    ProcessData *elements; // array of processes
    enum algorithm algo;   // used to define which algorithm we are dealing with
    int size;              // size of our elements array
} MinHeap;

MinHeap initMinHeap(enum algorithm a)
{
    MinHeap hp;
    hp.size = 0;
    hp.algo = a;
}

void swap(ProcessData *d1, ProcessData *d2)
{
    ProcessData temp = *d1;
    *d1 = *d2;
    *d2 = temp;
}

void heapify(MinHeap *hp, int i)
{
    if (hp->algo == HPF) // HPF    (priority)
    {
        int smallest = (LEFTCHILD(i) < hp->size && hp->elements[LEFTCHILD(i)].priority < hp->elements[i].priority) ? LEFTCHILD(i) : i;
        if (RIGHTCHILD(i) < hp->size && hp->elements[RIGHTCHILD(i)].priority < hp->elements[smallest].priority)
        {
            smallest = RIGHTCHILD(i);
        }
        if (smallest != i)
        {
            swap(&(hp->elements[i]), &(hp->elements[smallest]));
            heapify(hp, smallest);
        }
    }
    else // SRTN      (remaining time)
    {
        int smallest = (LEFTCHILD(i) < hp->size && hp->elements[LEFTCHILD(i)].remainingTime < hp->elements[i].remainingTime) ? LEFTCHILD(i) : i;
        if (RIGHTCHILD(i) < hp->size && hp->elements[RIGHTCHILD(i)].remainingTime < hp->elements[smallest].remainingTime)
        {
            smallest = RIGHTCHILD(i);
        }
        if (smallest != i)
        { // if it is not already at the correct place
            swap(&(hp->elements[i]), &(hp->elements[smallest]));
            heapify(hp, smallest);
        }
    }
}

void push(MinHeap *hp, ProcessData data)
{
    if (hp->size > 0)
    {
        hp->elements = realloc(hp->elements, (hp->size + 1) * sizeof(ProcessData));
    }
    else
    {
        hp->elements = malloc(sizeof(ProcessData));
    }

    ProcessData nd;
    nd.priority = data.priority;
    nd.remainingTime = data.remainingTime;
    nd.ID = data.ID;

    int i = (hp->size)++;
    if (hp->algo == HPF) // hpf
    {
        while (i && nd.priority < hp->elements[PARENT(i)].priority)
        {
            hp->elements[i] = hp->elements[PARENT(i)];
            i = PARENT(i);
        }
        hp->elements[i] = nd;
    }
    else // SRTN
    {
        while (i && nd.remainingTime < hp->elements[PARENT(i)].remainingTime)
        {
            hp->elements[i] = hp->elements[PARENT(i)];
            i = PARENT(i);
        }
        hp->elements[i] = nd;
    }
}

int Empty(MinHeap *hp)
{
    return hp->size == 0;
}

ProcessData *pop(MinHeap *hp)
{
    if (hp->size > 0)
    {
        ProcessData n;
        ProcessData *temp = &n;

        temp->ID = hp->elements[0].ID;
        temp->priority = hp->elements[0].priority;
        temp->remainingTime = hp->elements[0].remainingTime;

        hp->elements[0] = hp->elements[--(hp->size)];
        hp->elements = realloc(hp->elements, hp->size * sizeof(ProcessData));
        heapify(hp, 0);
        return temp;
    }
    else
    {
        free(hp->elements);
        return NULL;
    }
}

ProcessData *peek(MinHeap *hp)
{
    if (hp->size > 0)
    {
        ProcessData n;
        ProcessData *temp = &n;
        temp->ID = hp->elements[0].ID;
        temp->priority = hp->elements[0].priority;
        temp->remainingTime = hp->elements[0].remainingTime;
        return temp;
    }
    else
        return NULL;
}

int main()
{
    ProcessData data;
    data.ID = 1;
    data.priority = 20;
    data.remainingTime = 30;

    Node *q = newNode(data);
    Queue Q = init();

    enqueue(&Q, data);
    printf("  %d\n  %d\n  %d\n", front(&Q)->ID, front(&Q)->priority, front(&Q)->remainingTime); // testing out queue

    return 0;
}
