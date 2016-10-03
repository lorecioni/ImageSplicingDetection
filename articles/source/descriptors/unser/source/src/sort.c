#include "sort.h"

#include "queue.h"

int *CountingSort(int *val, int nelems, char order)
{
  int *sval=NULL;
  int *idx;
  int max, min;
  int i, v, n;

  min =  INT_MAX;
  max = -INT_MAX;
  for (i=0;i<nelems;i++){
    if (val[i] < min)
      min = val[i];
    if (val[i] > max)
      max = val[i];
  }

  n   = max-min+1;
  idx = AllocIntArray(n);
  memset(idx,0,n*sizeof(int));

  for (i=0;i<nelems;i++)
    idx[val[i]-min]++;

  for (i=1;i<n;i++)
    idx[i] += idx[i-1];

  sval = AllocIntArray(nelems);
  if (order==INCREASING){
    for (i=0;i<nelems;i++){
      v = val[i];
      sval[--idx[v-min]] = v;
    }
  }
  else { /* order = DECREASING */
    for (i=0;i<nelems;i++){
      v = val[i];
      sval[--idx[v-min]+nelems-1] = v;
    }
  }
  free(idx);

  return(sval);
}

void SelectionSort(int *val, int nelems, char order)
{
  int i,j,jmax;

  if (order == INCREASING){
    for (i=nelems-1; i > 0; i--){ 
      jmax = i;
      for (j=0; j < i; j++){
	if (val[j] > val[jmax])
	  jmax = j;
      }
      if (jmax != i)
	Change(sizeof(int),&val[i],&val[jmax]);
    }
  } else { /* DECREASING */
    for (i=0; i < nelems-1; i++){ 
      jmax = i;
      for (j=i+1; j < nelems; j++){
	if (val[j] > val[jmax])
	  jmax = j;
      }
      if (jmax != i)
	Change(sizeof(int),&val[i],&val[jmax]);
    }
  }
}

int *BucketSort(int *val, int nelems, char order)
{
  int i,j,maxval=INT_MIN,*sval=NULL;
  Queue *Q=NULL;

  for(i=0; i < nelems; i++) 
    if (val[i] > maxval)
      maxval = val[i];
  
  Q = CreateQueue(maxval+1,nelems);
  for(i=0; i < nelems; i++)
    InsertQueue(Q,val[i],i);

  sval = AllocIntArray(nelems);
  if (order==INCREASING){
    j = 0;
    while(!EmptyQueue(Q)) {
      i = RemoveQueue(Q);
      sval[j] = val[i];
      j++;
    }
  } else { /* order = DECREASING */
    j = nelems-1;
    while(!EmptyQueue(Q)) {
      i = RemoveQueue(Q);
      sval[j] = val[i];
      j--;
    }  
  }

  DestroyQueue(&Q);
  
  return(sval);
}

void QuickSort(void *basearg, size_t nel, size_t width, int (* compar)(const void*, const void*))
{
  int gap, ngap;
  int t1, t2;
  int i, j;
  char *jd;
  char *base = (char *)basearg;

  t1 = nel * width;
  for (ngap=nel/2;ngap>0;ngap=ngap/2) {
    gap = ngap * width;
    t2 = gap + width;
    jd = base + gap;
    for (i=t2;i<=t1;i+=width)
      for (j=i-t2;j>=0;j-=gap) {
        if ((*compar)(base + j, jd + j) <= 0)
          break;
        Change(width, base + j, jd + j);
      }
  }
}

