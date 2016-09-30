#ifndef _SET_H_
#define _SET_H_

typedef struct _set {
  int *p;
  int *order;
  int n;
} Set;

Set  *CreateSet(int n);
void  DestroySet(Set **set);
Set  *ReadSet(char *filename);
void  WriteSet(Set *set, char *filename);
void  MakeSet(Set *set, int x);
void  Union(Set *set, int x, int y);
void  Link(Set *set, int x, int y);
int   FindSet(Set *set, int x);

#endif
