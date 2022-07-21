#include "Data_structures.h"

Node *newNode(processData *data)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

Queue* Queue_init()
{
    Queue* q;
    q = malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}


processData* dequeue(Queue *q)
{
    if (q->size == 0)
        return NULL;
    Node *temp = q->front;
    q->front = q->front->next;

    //free(temp); 
    //I made a litlle fix so we can access the dequeued data
    //It's now the responsibillity of the caller to free the allocated memory

    if (q->front == NULL)
        q->rear = NULL;  //empty queue

    q->size -= 1;

    return temp->data;
}

processData *front(Queue *q)
{
    if (q->front != NULL)
        return q->front->data;
}

processData *rear(Queue *q)
{
    if (q->rear != NULL)
        return q->rear->data;
}

void enqueue(Queue *Q, processData *data)
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
}

int isEmpty(Queue *q)
{
    return q->size == 0;
}

int size_queue(Queue *q)
{
    return q->size;
}




PriorityQueue* PriorityQueue_init(ALGORITHM algo)
{
    PriorityQueue* hp;
    hp = malloc(sizeof(PriorityQueue));
    hp->size = 0;
    hp->algo = algo;
    return hp;
}

void swap(processData *d1, processData *d2)
{
    processData temp = *d1;
    *d1 = *d2;
    *d2 = temp;
}

void heapify(PriorityQueue *hp, int i)
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
    else if (hp->algo == SRTN)// SRTN      (remaining time)
    {
        int smallest = (LEFTCHILD(i) < hp->size && hp->elements[LEFTCHILD(i)].remainingtime < hp->elements[i].remainingtime) ? LEFTCHILD(i) : i;
        if (RIGHTCHILD(i) < hp->size && hp->elements[RIGHTCHILD(i)].remainingtime < hp->elements[smallest].remainingtime)
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

void push(PriorityQueue *hp, processData *data)
{
    if (hp->size > 0)
    {
        hp->elements = realloc(hp->elements, (hp->size + 1) * sizeof(processData));
    }
    else
    {
        hp->elements = malloc(sizeof(processData));
    }

    processData nd;
    nd.priority = data->priority;
    nd.remainingtime = data->remainingtime;
    nd.id = data->id;
    nd.runningtime=data->runningtime;
    nd.arrivaltime=data->arrivaltime;

    nd.memsize = data->memsize;

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
    else if (hp->algo == SRTN)// SRTN
    {
        while (i && nd.remainingtime < hp->elements[PARENT(i)].remainingtime)
        {
            hp->elements[i] = hp->elements[PARENT(i)];
            i = PARENT(i);
        }
        hp->elements[i] = nd;
    }
}

int Empty(PriorityQueue *hp)
{
    return hp->size == 0;
}

processData *pop(PriorityQueue *hp)
{
    if (hp->size > 0)
    {
        processData n;
        processData *temp = &n;

        temp->id = hp->elements[0].id;
        temp->priority = hp->elements[0].priority;
        temp->remainingtime = hp->elements[0].remainingtime;
        temp->runningtime=hp->elements[0].runningtime;
        temp->arrivaltime=hp->elements[0].arrivaltime;
        
        temp->memsize = hp->elements[0].memsize;

        hp->elements[0] = hp->elements[--(hp->size)];
        hp->elements = realloc(hp->elements, hp->size * sizeof(processData));
        heapify(hp, 0);
        return temp;
    }
    else
    {
        free(hp->elements);
        return NULL;
    }
}

processData *peek(PriorityQueue *hp)
{
    if (hp->size > 0)
    {
        processData n;
        processData *temp = &n;
        temp->id = hp->elements[0].id;
        temp->priority = hp->elements[0].priority;
        temp->remainingtime = hp->elements[0].remainingtime;
        temp->runningtime=hp->elements[0].runningtime;
        temp->arrivaltime=hp->elements[0].arrivaltime;
        
        return temp;
    }
    else
        return NULL;
}

int size_prioQ(PriorityQueue *hp)
{
    return hp->size;
}
