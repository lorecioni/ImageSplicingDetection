#include "acc.h"

#include "adjacency.h"
#include "common.h"
#include "image.h"

#define CDIM 4
#define SIZE (4 * CDIM * CDIM * CDIM)

typedef struct _Property {
  int color;
  int frequency[4];
} Property;

typedef struct _VisualFeature {
  ulong *colorH;
  int n;
} VisualFeature;

typedef struct _CompressedVisualFeature {
  uchar *colorH;
  int n;
} CompressedVisualFeature;

Property *AllocPropertyArray(int n)
{
  Property *v=NULL;
  v = (Property *) calloc(n,sizeof(Property));
  if (v==NULL)
    Error(MSG1,"AllocPropertyArray");
  return(v);
}

VisualFeature *CreateVisualFeature(int n)
{
  VisualFeature *vf=NULL;

  vf = (VisualFeature *) calloc(1,sizeof(VisualFeature));
  if (vf != NULL) {
    vf->colorH = AllocULongArray(n);
    vf->n = n;
  } else {
    Error(MSG1,"CreateVisualFeature");
  }
  return(vf);
}

void DestroyVisualFeature(VisualFeature **vf)
{
  VisualFeature *aux;

  aux = *vf;
  if (aux != NULL) {
    if (aux->colorH != NULL) free(aux->colorH);
    free(aux);
    *vf = NULL;
  }
}

CompressedVisualFeature *CreateCompressedVisualFeature(int n)
{
  CompressedVisualFeature *cvf=NULL;

  cvf = (CompressedVisualFeature *) calloc(1,sizeof(CompressedVisualFeature));
  if (cvf != NULL) {
    cvf->colorH = AllocUCharArray(n);
    cvf->n = n;
  } else {
    Error(MSG1,"CreateCompressedVisualFeature");
  }
  return(cvf);
}

void DestroyCompressedVisualFeature(CompressedVisualFeature **cvf)
{
  CompressedVisualFeature *aux;

  aux = *cvf;
  if (aux != NULL) {
    if (aux->colorH != NULL) free(aux->colorH);
    free(aux);
    *cvf = NULL;
  }
}

int *QuantizeColors(CImage *cimg, int color_dim)
{
  ulong i;
  ulong r, g, b;
  ulong fator_g, fator_b;
  int *color, n;
  
  n = cimg->C[0]->nrows * cimg->C[0]->ncols;  

  color = AllocIntArray(n);
  
  fator_g = color_dim;
  fator_b = fator_g*color_dim;
  
  for(i=0; i<n; i++){
    r = color_dim*cimg->C[0]->val[i]/256;
    g = color_dim*cimg->C[1]->val[i]/256;
    b = color_dim*cimg->C[2]->val[i]/256;
    
    color[i] = (r + fator_g*g + fator_b*b);
  }
  return(color);
}

uchar ComputeNorm(float value)
{
  return ((uchar)(255. * value));
}

void LinearNormalizeHistogram(uchar *ch, ulong *h, ulong max, int size)
{
  int i;
  uchar v;
  
  for(i=0; i<size; i++){
    v = ComputeNorm((float) h[i] / (float) max);
    ch[i] = (uchar)(v);
  }
}

uchar ComputeLog(float value)
{
  uchar result;
  
  value = 255. * value;
  if(value==0.)       result=0;
  else if(value<1.)   result=1;
  else if(value<2.)   result=2;
  else if(value<4.)   result=3;
  else if(value<8.)   result=4;
  else if(value<16.)  result=5;
  else if(value<32.)  result=6;
  else if(value<64.)  result=7;
  else if(value<128.) result=8;
  else                result=9;
  
  return(result);
}

void NonLinearNormalizeHistogram(uchar *ch, ulong *h, ulong max, int size)
{
  int i;
  uchar v;
  
  for(i=0; i<size; i++){
    v = ComputeLog((float) h[i] / (float) max);
    ch[i] = (uchar)('0' + v);
  }
}

void ComputeFrequencyProperty(Image *img, Property *ppt)
{ 
  ulong x, y, p, q;
  uchar d, r;
  AdjRel *A;
  Pixel v;
  int i;

  for (p=0; p<img->nrows*img->ncols; p++)
    for (d=0; d<4; d++)
      ppt[p].frequency[d] = 0;

  A = Circular(1.0);
  for(y=0; y<img->nrows; y++)
    for(x=0; x<img->ncols; x++){
      p = x + img->tbrow[y];
      for(r=1,d=0; r<=7; r+=2,d++)
        for (i=1; i < A->n; i++){
          v.x = x + r * A->dx[i]; 
          v.y = y + r * A->dy[i];
          if (ValidPixel(img,v.x,v.y)){
            q = v.x + img->tbrow[v.y];
            if(ppt[p].color == ppt[q].color)
	      ppt[p].frequency[d]++;
	  }
        }
    }
  DestroyAdjRel(&A);
}

Property *ComputePixelsProperties(CImage *cimg)
{
  Property *p=NULL;
  int *color, i, n;
  
  n = cimg->C[0]->nrows * cimg->C[0]->ncols;  
  
  p = AllocPropertyArray(n);
  
  color = QuantizeColors(cimg, CDIM);
  for(i=0; i<n; i++)
    p[i].color=color[i];
  ComputeFrequencyProperty(cimg->C[0], p);
  
  free(color);
  return(p);
}

VisualFeature *ComputeHistograms(Property *p, int npixels)
{
  VisualFeature *vf=NULL;
  ulong i, d;
  
  vf = CreateVisualFeature(SIZE);
  for(i=0; i<SIZE; i++)
    vf->colorH[i] = 0;

  
  for(d=0; d<4; d++)
    for(i=0; i<npixels; i++)
      vf->colorH[4 * p[i].color + d] += p[i].frequency[d];
  return(vf);
}

CompressedVisualFeature *NormalizeHistograms(VisualFeature *vf, int npixels)
{
  CompressedVisualFeature *cvf=NULL;
  
  cvf = CreateCompressedVisualFeature(SIZE);
  NonLinearNormalizeHistogram(cvf->colorH, vf->colorH, 4 * npixels, SIZE);
  
  return(cvf);
}

CompressedVisualFeature *ReadCompressedVisualFeatures(char *filename)
{
  CompressedVisualFeature *cvf=NULL;
  FILE *fp;
  int i, n;
  uchar c;

  fp = fopen(filename,"r");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%d\n",&n);
  cvf = CreateCompressedVisualFeature(n);
  for (i=0; i<n; i++) {
    fscanf(fp,"%c\n",&c);
    cvf->colorH[i] = c;
  }
  fclose(fp);
  return(cvf);
}

void WriteCompressedVisualFeatures(CompressedVisualFeature *cvf,char *filename)
{
  FILE *fp;
  int i;
  
  fp = fopen(filename,"w");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  
  fprintf(fp,"%d\n",SIZE);
  for(i=0;i<SIZE;i++)
    fprintf(fp, "%c", cvf->colorH[i]);
  
  fclose(fp);
}

CompressedVisualFeature *ExtractCompressedVisualFeatures(CImage *cimg)
{
  CompressedVisualFeature *cvf=NULL;
  VisualFeature *vf;
  Property *p;
  int npixels;
  
  npixels = cimg->C[0]->nrows * cimg->C[0]->ncols;
  
  p = ComputePixelsProperties(cimg);
  vf = ComputeHistograms(p, npixels);
  cvf = NormalizeHistograms(vf, npixels);
  
  free(p);
  DestroyVisualFeature(&vf);
  return(cvf);
}

Histogram *ACC(CImage *cimg)
{
  Histogram *h = NULL;
  CompressedVisualFeature *cvf;
  int i;

  cvf = ExtractCompressedVisualFeatures(cimg);

  h = CreateHistogram(SIZE);  
  for (i=0; i<SIZE; i++)
    h->v[i] = cvf->colorH[i];
  DestroyCompressedVisualFeature(&cvf);

  return(h);
}

