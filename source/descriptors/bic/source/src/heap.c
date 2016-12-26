#include "heap.h"
#include "common.h"

int dad(int i)
{
  return((i-1)/2);
}

int leftson(int i)
{
  return(2*i+1);
}

int rightson(int i)
{
  return(2*i+2);
}

/* Heap Functions */

void GoUp(Heap *H, int i)
{
  int j=dad(i);

  while((j >= 0) && (H->cost[H->pixel[j]] > H->cost[H->pixel[i]])){
    Change(sizeof(int),&H->pixel[j],&H->pixel[i]);
    H->pos[H->pixel[i]]=i;
    H->pos[H->pixel[j]]=j;
    i=j;
    j=dad(i);
  }
}

void GoDown(Heap *H, int i)
{
  int least, left=leftson(i), right=rightson(i);

  if ((left <= H->last)&&(H->cost[H->pixel[left]] < H->cost[H->pixel[i]]))
    least = left;
  else
    least = i;

  if ((right <= H->last)&&(H->cost[H->pixel[right]] < H->cost[H->pixel[least]]))
    least = right;

  if (least != i){
    Change(sizeof(int),&H->pixel[least],&H->pixel[i]);
    H->pos[H->pixel[i]]=i;
    H->pos[H->pixel[least]]=least;
    GoDown(H,least);
 }

}

bool HeapIsFull(Heap *H)
{
  if (H->last == (H->n-1))
    return(true);
  else
    return(false);
}

bool HeapIsEmpty(Heap *H)
{
  if (H->last == -1)
    return(true);
  else
    return(false);
}

Heap *CreateHeap(int n, int *cost)
{
  Heap *H=NULL;
  int i;

  if (cost == NULL){
    fprintf(stdout,"Cannot create heap without cost map\n");
    return(NULL);
  }

  H = (Heap *) calloc(1,sizeof(Heap));
  if (H != NULL) {
    H->n     = n;
    H->cost  = cost;
    H->color = (char *) calloc(n,sizeof(char));
    H->pixel = (int *) calloc(n,sizeof(int));
    H->pos   = (int *) calloc(n,sizeof(int));
    H->last = -1;
    if ((H->color == NULL) || (H->pos == NULL) || (H->pixel == NULL)){
      fprintf(stderr,"Insufficient Memory in CreateHeap\n");
      exit(-1);
    }
    for (i=0; i < H->n; i++){
      H->color[i]=WHITE;
      H->pos[i]=-1;
      H->pixel[i]=-1;
    }    
  } else {
      fprintf(stderr,"Insufficient Memory in CreateHeap\n");
      exit(-1);
    }

  return(H);
}

void DestroyHeap(Heap **H)
{
  Heap *aux;
  
  aux = *H;
  if (aux != NULL) {
    if (aux->pixel != NULL) free(aux->pixel);
    if (aux->color != NULL) free(aux->color);
    if (aux->pos != NULL)   free(aux->pos);
    free(aux);
    *H = NULL;
  }
}

bool InsertHeap(Heap *H, int pixel)
{
  if (!HeapIsFull(H)){
    H->last ++;
    H->pixel[H->last] = pixel;
    H->color[pixel]   = GRAY;
    H->pos[pixel]     = H->last;
    GoUp(H,H->last); 
    return(true);
  } else 
    return(false);
}

bool RemoveHeap(Heap *H, int *pixel)
{
  if (!HeapIsEmpty(H)){
    *pixel = H->pixel[0];
    H->pos[*pixel]=-1;
    H->color[*pixel] = BLACK;
    H->pixel[0] = H->pixel[H->last];
    H->last--;
    GoDown(H,0);
    return(true);
  } else 
    return(false);
}










