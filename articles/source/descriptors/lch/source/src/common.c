#include "common.h"

char *AllocCharArray(int n)
{
  char *v=NULL;
  v = (char *) calloc(n,sizeof(char));
  if (v == NULL)
    Error(MSG1,"AllocCharArray");
  return(v);
}

uchar *AllocUCharArray(int n)
{
  uchar *v=NULL;
  v = (uchar *) calloc(n,sizeof(uchar));
  if (v == NULL)
    Error(MSG1,"AllocUCharArray");
  return(v);
}

short *AllocUShortArray(int n)
{
  short *v=NULL;
  v = (short *) calloc(n,sizeof(short));
  if (v == NULL)
    Error(MSG1,"AllocUShortArray");
  return(v);
}

long *AllocULongArray(int n)
{
  long *v=NULL;
  v = (long *) calloc(n,sizeof(long));
  if (v == NULL)
    Error(MSG1,"AllocULongArray");
  return(v);
}

bool *AllocBoolArray(int n)
{
  bool *v=NULL;
  v = (bool *) calloc(n,sizeof(bool));
  if (v == NULL)
    Error(MSG1,"AllocBoolArray");
  return(v);
}

int *AllocIntArray(int n)
{
  int *v=NULL;
  v = (int *) calloc(n,sizeof(int));
  if (v == NULL)
    Error(MSG1,"AllocIntArray");
  return(v);
}

float *AllocFloatArray(int n)
{
  float *v=NULL;
  v = (float *) calloc(n,sizeof(float));
  if (v == NULL)
    Error(MSG1,"AllocFloatArray");
  return(v);
}

double *AllocDoubleArray(int n)
{
  double *v=NULL;
  v = (double *) calloc(n,sizeof(double));
  if (v == NULL)
    Error(MSG1,"AllocDoubleArray");
  return(v);
}

void Error(char *msg,char *func){ /* It prints error message and exits
                                    the program. */
  fprintf(stderr,"Error:%s in %s\n",msg,func);
  exit(-1);
}

void Warning(char *msg,char *func){ /* It prints warning message and
                                       leaves the routine. */
 fprintf(stdout,"Warning:%s in %s\n",msg,func);

}

void Change(size_t w, void *a, void *b){/* It changes content between a and b */
  char tmp;
  char *pa, *pb;

  pa = (char *)a;
  pb = (char *)b;
  while (w--) {
    tmp = *pa;
    *pa++ = *pb;
    *pb++ = tmp;
  }
}

int NCFgets(char *s, int m, FILE *f) {
  while(fgets(s,m,f)!=NULL)
    if (s[0]!='#') return 1;
  return 0;
}



