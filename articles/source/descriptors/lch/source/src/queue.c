#include "queue.h"

/* A priority queue Q consists of two data structures: a circular queue C and a table L that encodes all possible doubly-linked lists.

   Q requires that the maximum cost of an edge in the graph be a non-negative integer less than the number of buckets in C.
   Q->C.first[i] gives the first element that is in bucket i.
   Q->C.last[i]  gives the last  element that is in bucket i.
   Q->C.nbuckets gives the number of buckets in C.
   Q->C.current gives the current bucket being processed.
   Q->C.tiebreak gives the FIFO or LIFO tie breaking policies

   All possible doubly-linked lists are represented in L. Each bucket contains a doubly-linked list that is treated as a FIFO.
   Q->L.elem[i].next: the next element to i 
   Q->L.elem[i].prev: the previous element to i
   Q->L.elem[i].color: the color of i (WHITE=never inserted, GRAY=inserted,
                       BLACK=removed)
   Q->L.nelems: gives the total number of elements that can be inserted in Q (It is usually the number of pixels in a given image or the number of nodes in a graph)

   Insertions and updates are done in O(1).
   Removal may take O(Cmax+1), where Cmax+1 is the number of buckets.
*/

Queue *CreateQueue(int nbuckets, int nelems)
{
  Queue *Q=NULL;

  Q = (Queue *) malloc(1*sizeof(Queue));
  
  if (Q != NULL) {
    Q->C.first = (int *)malloc(nbuckets * sizeof(int));
    Q->C.last  = (int *)malloc(nbuckets * sizeof(int));
    Q->C.nbuckets = nbuckets;
    if ( (Q->C.first != NULL) && (Q->C.last != NULL) ){
      Q->L.elem = (Node *)malloc(nelems*sizeof(Node));
      Q->L.nelems = nelems;
      if (Q->L.elem != NULL){
	ResetQueue(Q);
      } else
	Error(MSG1,"CreateQueue");	
    } else
      Error(MSG1,"CreateQueue");
  } else 
    Error(MSG1,"CreateQueue");
  
  return(Q);
}

void ResetQueue(Queue *Q)
{
  unsigned int i;

  Q->C.current  = 0;
  SetTieBreak(Q,FIFOBREAK);
#if defined(__i386__) && (NIL==-1 || NIL==0)
  i = Q->C.nbuckets*sizeof(int);
  memset(Q->C.first, NIL, i);
  memset(Q->C.last,  NIL, i);
  memset(Q->L.elem,  NIL, Q->L.nelems*sizeof(Node));
  for (i=0; i<Q->L.nelems; ++i)
    Q->L.elem[i].color = WHITE;
#else
  for (i=0; i < Q->C.nbuckets; ++i)
    Q->C.first[i]=Q->C.last[i]=NIL;
  for (i=0; i < Q->L.nelems; ++i) {
    Q->L.elem[i].next =  Q->L.elem[i].prev = NIL;
    Q->L.elem[i].color = WHITE;
  }
#endif

}

void DestroyQueue(Queue **Q)
{
  Queue *aux;

  aux = *Q;
  if (aux != NULL) {
    if (aux->C.first != NULL) free(aux->C.first);
    if (aux->C.last  != NULL) free(aux->C.last);
    if (aux->L.elem  != NULL) free(aux->L.elem);
    free(aux);
    *Q = NULL;
  }
}

void InsertQueue(Queue *Q, int bucket, int elem)
{
  if (Q->C.first[bucket] == NIL){ 
    Q->C.first[bucket]   = elem;  
    Q->L.elem[elem].prev = NIL;
  }else {
    Q->L.elem[Q->C.last[bucket]].next = elem;
    Q->L.elem[elem].prev = Q->C.last[bucket];
  }
  
  Q->C.last[bucket]     = elem;
  Q->L.elem[elem].next  = NIL;
  Q->L.elem[elem].color = GRAY;
}

int RemoveQueue(Queue *Q)
{
  int elem=NIL, next, prev;
  int last;

  /** moves to next element or returns EMPTY queue **/
  if (Q->C.first[Q->C.current] == NIL) {
    last = Q->C.current;
    
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    
    while ((Q->C.first[Q->C.current] == NIL) && (Q->C.current != last)) {
      Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    }
    
    if (Q->C.first[Q->C.current] == NIL)
      return NIL;
  }

  if (Q->C.tiebreak == LIFOBREAK) {
    elem = Q->C.last[Q->C.current];
  
    prev = Q->L.elem[elem].prev;
    if (prev == NIL) {         /* there was a single element in the list */
      Q->C.last[Q->C.current] = Q->C.first[Q->C.current]  = NIL;    
    }
    else {
      Q->C.last[Q->C.current] = prev;
      Q->L.elem[prev].next = NIL;
    }
  } else { /* Assume FIFO policy for breaking ties */
    elem = Q->C.first[Q->C.current];
  
    next = Q->L.elem[elem].next;
    if (next == NIL) {         /* there was a single element in the list */
      Q->C.first[Q->C.current] = Q->C.last[Q->C.current]  = NIL;    
    }
    else {
      Q->C.first[Q->C.current] = next;
      Q->L.elem[next].prev = NIL;
    }
  }

  Q->L.elem[elem].color = BLACK;

  return elem;
}

void RemoveQueueElem(Queue *Q, int elem, int bucket)
{
  int prev,next;

  prev = Q->L.elem[elem].prev;
  next = Q->L.elem[elem].next;
  
  /* if elem is the first element */
  if (Q->C.first[bucket] == elem) {
    Q->C.first[bucket] = next;
    if (next == NIL) /* elem is also the last one */
      Q->C.last[bucket] = NIL;
    else
      Q->L.elem[next].prev = NIL;
  }
  else{   /* elem is in the middle or it is the last */
    Q->L.elem[prev].next = next;
    if (next == NIL) /* if it is the last */
      Q->C.last[bucket] = prev;
    else 
      Q->L.elem[next].prev = prev;
  }

  Q->L.elem[elem].color = BLACK;
}

void UpdateQueue(Queue *Q, int elem, int from, int to)
{
  RemoveQueueElem(Q, elem, from);
  InsertQueue(Q, to, elem);
}

int EmptyQueue(Queue *Q)
{
  int last;
  if (Q->C.first[Q->C.current] != NIL)
    return 0;
  
  last = Q->C.current;
  
  Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
  
  while ((Q->C.first[Q->C.current] == NIL) && (Q->C.current != last)) {
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets); 
  }
  
  return (Q->C.first[Q->C.current] == NIL);
}








