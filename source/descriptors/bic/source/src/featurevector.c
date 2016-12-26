#include "featurevector.h"

FeatureVector1D *CreateFeatureVector1D(int n)
{
  FeatureVector1D *fv=NULL;
  
  fv = (FeatureVector1D *) calloc(1,sizeof(FeatureVector1D));
  if (fv != NULL) {
    fv->X = AllocDoubleArray(n);
    fv->n = n;
  } else {
    Error(MSG1,"CreateFeatureVector");
  }
  return(fv);
}

void DestroyFeatureVector1D(FeatureVector1D **fv)
{
  FeatureVector1D *aux;
  
  aux = *fv;
  if (aux != NULL){
    if (aux->X != NULL) free(aux->X);
    free(aux);
    *fv = NULL;
  }
}

Ap_FeatureVector1D *AllocAp_FeatureVector1DArray(int n)
{
  Ap_FeatureVector1D *v=NULL;
  v = (Ap_FeatureVector1D *) calloc(n,sizeof(Ap_FeatureVector1D));
  if (v == NULL)
    Error(MSG1,"AllocAp_FeatureVector1DArray");
  return(v);
}

FeatureVector1D *ReadStreamFeatureVector1D(FILE *fp)
{
  FeatureVector1D *fv = NULL;
  int i, nbins;
  double x;

  if (fp != NULL) {
    fscanf(fp, "%d\n", &nbins);
    fv = CreateFeatureVector1D(nbins);
    for (i=0; i<nbins; i++){
      fscanf(fp, "%lf\n", &x);
      fv->X[i] = x;
    }
  }
  return(fv);
}

FeatureVector1D *ReadFileFeatureVector1D(char *filename)
{
  FeatureVector1D *fv = NULL;
  int i, nbins;
  double x;
  FILE *fp;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }

  fscanf(fp, "%d\n", &nbins);
  fv = CreateFeatureVector1D(nbins);
  for (i=0; i<nbins; i++){
    fscanf(fp, "%lf\n", &x);
    fv->X[i] = x;
  }
  fclose(fp);

  return(fv);
}

Ap_FeatureVector1D *ReadAp_FeatureVector1D(char *filename,int *n)
{
  Ap_FeatureVector1D *ap_fv = NULL;
  FILE *fp;
  int i;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp, "%d\n", n);
  ap_fv = AllocAp_FeatureVector1DArray(*n);
  for (i=0;i<*n;i++)
    ap_fv[i] = ReadStreamFeatureVector1D(fp);
  fclose(fp);

  return(ap_fv);
}

void WriteStreamFeatureVector1D(FeatureVector1D *fv,FILE *fp)
{
  int i;
  
  if (fp != NULL){
    fprintf(fp,"%d\n",fv->n);
    for (i=0; i < fv->n; i++)
      fprintf(fp,"%f\n",fv->X[i]);
  }
}

void WriteFileFeatureVector1D(FeatureVector1D *fv,char *filename)
{
  FILE *fp;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  WriteStreamFeatureVector1D(fv, fp);
  fclose(fp);
}

void WriteAp_FeatureVector1D(Ap_FeatureVector1D *ap_fv,int n,char *filename)
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
    WriteStreamFeatureVector1D(ap_fv[i], fp);
  fclose(fp);
}

FeatureVector1D *CopyFeatureVector1D(FeatureVector1D *fv)
{
  FeatureVector1D *fvc;
  
  fvc = CreateFeatureVector1D(fv->n);
  memcpy(fvc->X,fv->X,fv->n*sizeof(double));
   
  return(fvc);
}

void SortFeatureVector1D(FeatureVector1D *fv, int left, int right, char order)
{
  int pivot;
  
  if (left < right) {
    pivot = PartFeatureVector1D(fv,left,right,order);
    SortFeatureVector1D(fv,left,pivot-1,order);
    SortFeatureVector1D(fv,pivot+1,right,order); 
  }
}

int PartFeatureVector1D(FeatureVector1D *fv, int left, int right, char order)
{
  double x;
  int i;
  double X;
  
  x = fv->X[left];
  i = left;
  
  do {
    if (order == INCREASING){
      while ((fv->X[left] <= x)&&(left <= right)) left++;
      while (fv->X[right]  > x) right--;
    } else { /* order = DECREASING */
      while ((fv->X[left] >= x)&&(left <= right)) left++;
      while (fv->X[right]  < x) right--;
    }
    if (left < right){
      X = fv->X[left];
      fv->X[left]  = fv->X[right];
      fv->X[right] = X;
      left++; right--;
    }
  } while (left <= right);
  
  left = i;
  
  if (left != right){
    X = fv->X[left];
    fv->X[left]  = fv->X[right];
    fv->X[right] = X;
  }
  
  return (right);
}

FeatureVector2D *CreateFeatureVector2D(int n)
{
  FeatureVector2D *fv=NULL;
  
  fv = (FeatureVector2D *) calloc(1,sizeof(FeatureVector2D));
  if (fv != NULL) {
    fv->X = AllocDoubleArray(n);
    fv->Y = AllocDoubleArray(n);
    fv->n = n;
  } else {
    Error(MSG1,"CreateFeatureVector");
  }
  return(fv);
}

void DestroyFeatureVector2D(FeatureVector2D **fv)
{
  FeatureVector2D *aux;
  
  aux = *fv;
  if (aux != NULL){
    if (aux->X != NULL) free(aux->X);
    if (aux->Y != NULL) free(aux->Y);
    free(aux);
    *fv = NULL;
  }
}

Ap_FeatureVector2D *AllocAp_FeatureVector2DArray(int n)
{
  Ap_FeatureVector2D *v=NULL;
  v = (Ap_FeatureVector2D *) calloc(n,sizeof(Ap_FeatureVector2D));
  if (v == NULL)
    Error(MSG1,"AllocAp_FeatureVector2DArray");
  return(v);
}

FeatureVector2D *ReadStreamFeatureVector2D(FILE *fp)
{
  FeatureVector2D *fv = NULL;
  int i, nbins;
  double x,y;

  if (fp != NULL){
    fscanf(fp, "%d\n", &nbins);
    fv = CreateFeatureVector2D(nbins);
    for (i=0; i<nbins; i++){
      fscanf(fp, "%lf\t%lf\n", &x,&y);
      fv->X[i] = x;
      fv->Y[i] = y;
    }
  }
  return(fv);
}

FeatureVector2D *ReadFileFeatureVector2D(char *filename)
{
  FeatureVector2D *fv = NULL;
  int i, nbins;
  double x,y;
  FILE *fp;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }

  fscanf(fp, "%d\n", &nbins);
  fv = CreateFeatureVector2D(nbins);
  for (i=0; i<nbins; i++){
    fscanf(fp, "%lf\t%lf\n", &x,&y);
    fv->X[i] = x;
    fv->Y[i] = y;
  }
  fclose(fp);

  return(fv);
}

Ap_FeatureVector2D *ReadAp_FeatureVector2D(char *filename,int *n)
{
  Ap_FeatureVector2D *ap_fv = NULL;
  FILE *fp;
  int i;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp, "%d\n", n);
  ap_fv = AllocAp_FeatureVector2DArray(*n);
  for (i=0;i<*n;i++)
    ap_fv[i] = ReadStreamFeatureVector2D(fp);
  fclose(fp);

  return(ap_fv);
}

void WriteStreamFeatureVector2D(FeatureVector2D *fv,FILE *fp)
{
  int i;
  
  if (fp != NULL){
    fprintf(fp,"%d\n",fv->n);
    for (i=0; i < fv->n; i++)
      fprintf(fp,"%f\t%f\n",fv->X[i],fv->Y[i]);
  }
}

void WriteFileFeatureVector2D(FeatureVector2D *fv,char *filename)
{
  FILE *fp;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  WriteStreamFeatureVector2D(fv, fp);
  fclose(fp);
}

void WriteAp_FeatureVector2D(Ap_FeatureVector2D *ap_fv,int n,char *filename)
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
    WriteStreamFeatureVector2D(ap_fv[i], fp);
  fclose(fp);
}

FeatureVector2D *CopyFeatureVector2D(FeatureVector2D *fv)
{
  FeatureVector2D *fvc;
  
  fvc = CreateFeatureVector2D(fv->n);
  memcpy(fvc->X,fv->X,fv->n*sizeof(double));
  memcpy(fvc->Y,fv->Y,fv->n*sizeof(double));
  
  return(fvc);
}

void SortFeatureVector2D(FeatureVector2D *fv, int left, int right, char order)
{
  int pivot;
  
  if (left < right) {
    pivot = PartFeatureVector2D(fv,left,right,order);
    SortFeatureVector2D(fv,left,pivot-1,order);
    SortFeatureVector2D(fv,pivot+1,right,order); 
  }
}

int PartFeatureVector2D (FeatureVector2D *fv, int left, int right, char order)
{
  double y;
  int i;
  double X,Y;
  
  y = fv->Y[left];
  i = left;
  
  do {
    if (order == INCREASING){
      while ((fv->Y[left] <= y)&&(left <= right)) left++;
      while (fv->Y[right]  > y) right--;
    } else { /* order = DECREASING */
      while ((fv->Y[left] >= y)&&(left <= right)) left++;
      while (fv->Y[right]  < y) right--;
    }
    if (left < right){
      X = fv->X[left];
      Y = fv->Y[left];
      fv->X[left]  = fv->X[right];
      fv->Y[left]  = fv->Y[right];
      fv->X[right] = X;
      fv->Y[right] = Y;
      left++; right--;
    }
  } while (left <= right);
  
  left = i;
  
  if (left != right){
    X = fv->X[left];
    Y = fv->Y[left];
    fv->X[left]  = fv->X[right];
    fv->Y[left]  = fv->Y[right];
    fv->X[right] = X;
    fv->Y[right] = Y;
  }
  
  return (right);
}

void FeatureVector2DInvertXY(FeatureVector2D *fv)
{
  double tmp;
  int i;
  for (i=0; i<fv->n; i++){
    tmp = fv->X[i];
    fv->X[i] = fv->Y[i];
    fv->Y[i] = tmp;
  }
}

