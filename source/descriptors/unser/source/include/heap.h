#ifndef _HEAP_H_
#define _HEAP_H_

#include "common.h"

typedef struct _heap {
  int *cost;
  char *color;
  int *pixel;
  int *pos;
  int last;
  int n;
} Heap;

/* Auxiliary Functions */

int  dad(int i);
int  leftson(int i);
int  rightson(int i);

/* Heap Functions */

bool HeapIsFull(Heap *H);
bool HeapIsEmpty(Heap *H);
Heap *CreateHeap(int n, int *cost);
void DestroyHeap(Heap **H);
bool InsertHeap(Heap *H, int pixel);
bool RemoveHeap(Heap *H, int *pixel);
void GoUp(Heap *H, int i);
void GoDown(Heap *H, int i);


#endif











