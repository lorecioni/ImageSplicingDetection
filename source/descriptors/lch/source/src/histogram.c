#include "histogram.h"

Histogram *CreateHistogram(int n)
{
  Histogram *h=NULL;
  
  h = (Histogram *) calloc(1,sizeof(Histogram));
  if (h != NULL) {
    h->v = AllocUCharArray(n);
    h->n = n;
  } else {
    Error(MSG1,"CreateHistogram");
  }
  return(h);
}

void DestroyHistogram(Histogram **h)
{
  Histogram *aux;
  
  aux = *h;
  if (aux != NULL){
    if (aux->v != NULL) free(aux->v);
    free(aux);
    *h = NULL;
  }
}

Ap_Histogram *AllocAp_HistogramArray(int n)
{
  Ap_Histogram *v=NULL;
  v = (Ap_Histogram *) calloc(n,sizeof(Ap_Histogram));
  if (v == NULL)
    Error(MSG1,"AllocAp_HistogramArray");
  return(v);
}

Histogram *ReadStreamHistogram(FILE *fp)
{
  Histogram *h = NULL;
  int i, nbins;
  uchar c;

  if (fp != NULL) {
    fscanf(fp, "%d\n", &nbins);
    h = CreateHistogram(nbins);
    for (i=0; i<nbins; i++){
      fscanf(fp, "%c", &c);
      h->v[i] = c;
    }
  }
  return(h);
}

Histogram *ReadFileHistogram(char *filename)
{
  Histogram *h = NULL;
  int i, nbins;
  FILE *fp;
  uchar c;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }

  fscanf(fp, "%d\n", &nbins);
  h = CreateHistogram(nbins);
  for (i=0; i<nbins; i++){
    fscanf(fp, "%c", &c);
    h->v[i] = c;
  }
  fclose(fp);

  return(h);
}

Ap_Histogram *ReadAp_Histogram(char *filename,int *n)
{
  Ap_Histogram *ap_h = NULL;
  FILE *fp;
  int i;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp, "%d\n", n);
  ap_h = AllocAp_HistogramArray(*n);
  for (i=0;i<*n;i++)
    ap_h[i] = ReadStreamHistogram(fp);
  fclose(fp);

  return(ap_h);
}

void WriteStreamHistogram(Histogram *h,FILE *fp)
{
  int i;
  
  if (fp != NULL){
    fprintf(fp,"%d\n",h->n);
    for (i=0; i < h->n; i++)
      fprintf(fp,"%d ",h->v[i]);
    fprintf(fp,"\n");
  }
}

void WriteFileHistogram(Histogram *h,char *filename)
{
  FILE *fp;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  WriteStreamHistogram(h, fp);
  fclose(fp);
}

void WriteAp_Histogram(Ap_Histogram *ap_h,int n,char *filename)
{
  FILE *fp;
  int i;

  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fprintf(fp, "%d\n", n);
  for (i=0;i<n;i++)
    WriteStreamHistogram(ap_h[i], fp);
  fclose(fp);
}

Histogram *CopyHistogram(Histogram *h)
{
  Histogram *hc;
  
  hc = CreateHistogram(h->n);
  memcpy(hc->v,h->v,h->n*sizeof(double));
   
  return(hc);
}

long L1Distance(Histogram *h1, Histogram *h2) {
  long i;
  long sum = 0;
  long z = 0;
  
  for (i = 0; i < h1->n ; i++){
    z = fabs(h1->v[i] - h2->v[i]); 
    sum = sum + z;
  }
  return (sum);
}

long L2Distance(Histogram *h1, Histogram *h2) {
  long i;
  long sum = 0;
  long z = 0;
  
  for (i = 0; i < h1->n ; i++){
    z = h1->v[i] - h2->v[i]; 
    sum += z*z;
  }
  return (sum);
}

long LMDistance(Histogram *h1, Histogram *h2) {
  long i;
  long max = 0;
  long z = 0;
  
  for (i = 0; i < h1->n ; i++){
    z = fabs(h1->v[i] - h2->v[i]); 
    if (max < z) max = z;
  }
  return (max);
}

