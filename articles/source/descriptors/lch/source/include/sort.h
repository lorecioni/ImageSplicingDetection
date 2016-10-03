#ifndef _SORT_H_
#define _SORT_H_

#include "common.h"

int   *CountingSort(int *val, int nelems, char order);
int   *BucketSort(int *val, int nelems, char order);
void   SelectionSort(int *val, int nelems, char order); /* in place */
void   QuickSort(void *basearg, size_t nel, size_t width, int (* compar)(const void*, const void*));

#endif
