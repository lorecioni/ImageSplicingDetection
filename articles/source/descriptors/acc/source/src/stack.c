#include "stack.h"

void PushStack(Stack **S, int elem)
{
  Stack *p=NULL;

  p = (Stack *) calloc(1,sizeof(Stack));
  if (p == NULL) Error(MSG1,"Push");
  if (*S == NULL){
    p->elem  = elem;
    p->next  = NULL;
  }else{
    p->elem  = elem;
    p->next  = *S;
  }
  *S = p;
}

int PopStack(Stack **S)
{
  Stack *p;
  int elem=NIL;
  
  if (*S != NULL){
    p    =  *S;
    elem = p->elem;
    *S   = p->next;
    free(p);
  }

  return(elem);
}

void DestroyStack(Stack **S)
{
  Stack *p;
  while(*S != NULL){
    p = *S;
    *S = p->next;
    free(p);
  }
}

