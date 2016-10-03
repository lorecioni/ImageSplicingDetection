#include "adjacency.h"

AdjRel *CreateAdjRel(int n)
{
  AdjRel *A=NULL;

  A = (AdjRel *) calloc(1,sizeof(AdjRel));
  if (A != NULL){
    A->dx = AllocIntArray(n);
    A->dy = AllocIntArray(n);
    A->n  = n;
  } else {
    Error(MSG1,"CreateAdjRel");
  }

  return(A);
}

void DestroyAdjRel(AdjRel **A)
{
  AdjRel *aux;

  aux = *A;
  if (aux != NULL){
    if (aux->dx != NULL) free(aux->dx);
    if (aux->dy != NULL) free(aux->dy);
    free(aux);
    *A = NULL;
  }   
}

AdjRel *RightSide(AdjRel *A)
{
  AdjRel *R=NULL;
  int i;
  float d;

  /* Let p -> q be an arc represented by the increments dx,dy. Its
     right side is given by the increments Dx = -dy/d + dx/2 and Dy =
     dx/d + dy/2, where d=sqrt(dx²+dy²). */

  R = CreateAdjRel(A->n);
  for (i=0; i < R->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      R->dx[i] = ROUND(((float)A->dx[i]/2.0)-((float)A->dy[i]/d));
      R->dy[i] = ROUND(((float)A->dx[i]/d)+((float)A->dy[i]/2.0));
    }
  }

  return(R);
}

AdjRel *LeftSide(AdjRel *A)
{
  AdjRel *L=NULL;
  int i;
  float d;

  /* Let p -> q be an arc represented by the increments dx,dy. Its
     left side is given by the increments Dx = dy/d + dx/2 and Dy =
     -dx/d + dy/2, where d=sqrt(dx²+dy²). */

  L = CreateAdjRel(A->n);
  for (i=0; i < L->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      L->dx[i] = ROUND(((float)A->dx[i]/2.0)+((float)A->dy[i]/d));
      L->dy[i] = ROUND(((float)A->dy[i]/2)-((float)A->dx[i]/d));
    }
  }
  
  return(L);
}

AdjRel *RightSide2(AdjRel *A, float r)
{
  AdjRel *R=NULL;
  int i;
  float d;

  /* Let p -> q be an arc represented by the increments dx,dy. Its
     right side is given by the increments Dx = -dy/d + dx/2 and Dy =
     dx/d + dy/2, where d=sqrt(dx²+dy²). */

  R = CreateAdjRel(A->n);
  for (i=0; i < R->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      R->dx[i] = ROUND( ((float)A->dx[i]/2.0)-((float)A->dy[i]/d)*r );
      R->dy[i] = ROUND( ((float)A->dx[i]/d)+((float)A->dy[i]/2.0)*r );
    }
  }

  return(R);
}

AdjRel *LeftSide2(AdjRel *A, float r)
{
  AdjRel *L=NULL;
  int i;
  float d;

  /* Let p -> q be an arc represented by the increments dx,dy. Its
     left side is given by the increments Dx = dy/d + dx/2 and Dy =
     -dx/d + dy/2, where d=sqrt(dx²+dy²). */

  L = CreateAdjRel(A->n);
  for (i=0; i < L->n; i++){
    d  = sqrt(A->dx[i]*A->dx[i] + A->dy[i]*A->dy[i]);
    if (d != 0){
      L->dx[i] = ROUND( ((float)A->dx[i]/2.0)+((float)A->dy[i]/d)*r );
      L->dy[i] = ROUND( ((float)A->dy[i]/2)-((float)A->dx[i]/d)*r );
    }
  }
  
  return(L);
}


AdjRel *Circular(float r)
{
  AdjRel *A=NULL;
  int i,j,k,n,dx,dy,r0,r2,d,i0=0;
  float *da,*dr,aux;

  n=0;

  r0 = (int)r;
  r2  = (int)(r*r);
  for(dy=-r0;dy<=r0;dy++)
    for(dx=-r0;dx<=r0;dx++)
      if(((dx*dx)+(dy*dy)) <= r2)
	n++;
	
  A = CreateAdjRel(n);
  i=0;
  for(dy=-r0;dy<=r0;dy++)
    for(dx=-r0;dx<=r0;dx++)
      if(((dx*dx)+(dy*dy)) <= r2){
	A->dx[i]=dx;
	A->dy[i]=dy;
	if ((dx==0)&&(dy==0))
	  i0 = i;
	i++;
      }

  /* Set clockwise */
  
  da = AllocFloatArray(A->n);
  dr = AllocFloatArray(A->n);
  for (i=0; i < A->n; i++) {
    dx = A->dx[i];
    dy = A->dy[i];
    dr[i] = (float)sqrt((dx*dx) + (dy*dy));
    if (i != i0){ 
      da[i] = atan2(-dy,-dx)*180.0/PI;
      if (da[i] < 0.0)
	da[i] += 360.0;
    }
  }
  da[i0] = 0.0;
  dr[i0] = 0.0;

  /* place central pixel at first */
  
  aux    = da[i0];
  da[i0] = da[0];
  da[0]  = aux;
  aux    = dr[i0];
  dr[i0] = dr[0];
  dr[0]  = aux;
  d         = A->dx[i0];
  A->dx[i0] = A->dx[0];
  A->dx[0]  = d;
  d         = A->dy[i0];
  A->dy[i0] = A->dy[0];
  A->dy[0]  = d;

  /* sort by angle */
  
  for (i=1; i < A->n-1; i++){
    k = i;
    for (j=i+1; j < A->n; j++)
      if (da[j] < da[k]){
	k = j;
      }
    aux   = da[i];
    da[i] = da[k];
    da[k] = aux;
    aux   = dr[i];
    dr[i] = dr[k];
    dr[k] = aux;
    d   = A->dx[i];
    A->dx[i] = A->dx[k];
    A->dx[k] = d;
    d        = A->dy[i];
    A->dy[i] = A->dy[k];
    A->dy[k] = d;
  }

  /* sort by radius for each angle */
  
  for (i=1; i < A->n-1; i++){
    k = i;
    for (j=i+1; j < A->n; j++)
      if ((dr[j] < dr[k])&&(da[j]==da[k])){
	k = j;
      }
    aux   = dr[i];
    dr[i] = dr[k];
    dr[k] = aux;
    d        = A->dx[i];
    A->dx[i] = A->dx[k];
    A->dx[k] = d;
    d        = A->dy[i];
    A->dy[i] = A->dy[k];
    A->dy[k] = d;
  }

  free(dr);
  free(da);

  return(A);
}

AdjRel *Horizontal(int r)
{
  AdjRel *A=NULL;
  int i,n,dx;

  n=2*r+1;

  A = CreateAdjRel(n);
  i=1;
  for(dx=-r;dx<=r;dx++){
    if (dx != 0){
      A->dx[i]=dx;
      A->dy[i]=0;
      i++;
    }
  }
  /* place the central pixel at first */
  A->dx[0] = 0;
  A->dy[0] = 0;

  return(A);
}

AdjRel *Box(int ncols, int nrows)
{
  AdjRel *A=NULL;
  int i,dx,dy;

  if (ncols%2 == 0) ncols++;
  if (nrows%2 == 0) nrows++;

  A = CreateAdjRel(ncols*nrows);
  i=1;
  for(dy=-nrows/2;dy<=nrows/2;dy++){
    for(dx=-ncols/2;dx<=ncols/2;dx++){
      if ((dx != 0)||(dy != 0)){
	A->dx[i]=dx;
	A->dy[i]=dy;
	i++;
      }
    }
  }
  /* place the central pixel at first */
  A->dx[0] = 0;
  A->dy[0] = 0;

  return(A);
}

AdjRel *Vertical(int r)
{
  AdjRel *A=NULL;
  int i,n,dy;

  n=2*r+1;

  A = CreateAdjRel(n);
  i=1;
  for(dy=-r;dy<=r;dy++){
    if (dy != 0){
      A->dy[i]=dy;
      A->dx[i]=0;
      i++;
    }
  }
  /* place the central pixel at first */
  A->dx[0] = 0;
  A->dy[0] = 0;
  return(A);
}

AdjPxl *AdjPixels(Image *img, AdjRel *A)
{
  AdjPxl *N;
  int i;

  N = (AdjPxl *) calloc(1,sizeof(AdjPxl));
  if(N != NULL){
    N->dp = AllocIntArray(A->n);
    N->n  = A->n;
    for (i=0; i < N->n; i++)
      N->dp[i] = A->dx[i] + img->ncols*A->dy[i];
  }else{
    Error(MSG1,"AdjPixels");  
  }

  return(N);
}

void DestroyAdjPxl(AdjPxl **N)
{
  AdjPxl *aux;

  aux = *N;
  if (aux != NULL){
    if (aux->dp != NULL) free(aux->dp);
    free(aux);
    *N = NULL;
  }
}


int FrameSize(AdjRel *A)
{
  int sz=INT_MIN,i=0;

  for (i=0; i < A->n; i++){
    if (fabs(A->dx[i]) > sz) 
      sz = fabs(A->dx[i]);
    if (fabs(A->dy[i]) > sz) 
      sz = fabs(A->dy[i]);
  }
  return(sz);
}

AdjRel *ComplAdj(AdjRel *A1, AdjRel *A2)
{
  AdjRel *A;
  int i,j,n;
  char *subset=NULL;

  if (A1->n > A2->n){
    A  = A1;
    A1 = A2;
    A2 = A;
  }

  A = NULL;
  subset = AllocCharArray(A2->n);
  n = 0;
  for (i=0; i < A1->n; i++) 
    for (j=0; j < A2->n; j++)
      if ((A1->dx[i]==A2->dx[j])&&(A1->dy[i]==A2->dy[j])){	  
	subset[j] = 1;
	n++;
	break;
      }
  n = A2->n - n;
  
  if (n == 0) /* A1 == A2 */    
    return(NULL);

  A = CreateAdjRel(n);
  j=0;
  for (i=0; i < A2->n; i++) 
    if (subset[i] == 0){
      A->dx[j] = A2->dx[i];
      A->dy[j] = A2->dy[i];
      j++;
    }

  free(subset);

  return(A);
}

AdjRel *ShearedBox(int xsize, int ysize, float Si, float Sj)
{
  int i,usize,vsize;
  Pixel c,p;
  AdjRel *A = NULL;

  // shear: d(Sij+1)

  usize = ROUND(xsize*(fabs(Si)+1.0));
  vsize = ROUND(ysize*(fabs(Sj)+1.0));

  A = Box(usize,vsize);

  // position of the center of the voxel
  if (Si > 0) {
    c.x = xsize*(Si+0.5);
  } else {
    c.x = xsize*(0.5);    
  }
  if (Sj > 0) {
    c.y = ysize*(Sj+0.5);
  } else {
    c.y = ysize*(0.5);
  }

  i = 0;
  for (p.y=0;p.y< vsize;p.y++) {
     for (p.x=0;p.x< usize;p.x++) {
      A->dx[i] = p.x - c.x;
      A->dy[i] = p.y - c.y;
      i++;
     }	
  }
  return(A);
}
