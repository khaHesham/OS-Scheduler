#ifndef MEMORY_H

#define MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAXLEVEL 10

typedef struct MNode
{
    int index;
    struct MNode* next;

}MNode;

MNode* free_lists[MAXLEVEL + 1];


void initMemory();

int rec_allocate(int bucket);

int allocate(int size);

void rec_deallocate(int index, int bucket);

void deallocate(int index, int size);

#endif