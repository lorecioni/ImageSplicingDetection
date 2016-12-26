#ifndef _ADJACENCY_H_
#define _ADJACENCY_H_

#include "image.h"

typedef struct _adjrel {
  int *dx;
  int *dy;
  int n;
} AdjRel;

typedef struct _adjpxl {
  int *dp;
  int n;
} AdjPxl;


/* All adjacency relations must place the central pixel at first. The
   central pixel must be skipped when visiting the neighbors during
   the IFT. It must be considered in the other operations. */

AdjRel *CreateAdjRel(int n);
void    DestroyAdjRel(AdjRel **A);
AdjRel *RightSide(AdjRel *A);
AdjRel *LeftSide(AdjRel *A);
AdjRel *RightSide2(AdjRel *A, float r);
AdjRel *LeftSide2(AdjRel *A, float r);
AdjRel *Circular(float r);
AdjPxl *AdjPixels(Image *img,AdjRel *A);
void    DestroyAdjPxl(AdjPxl **N);
int     FrameSize(AdjRel *A);
AdjRel *ComplAdj(AdjRel *A1, AdjRel *A2);
AdjRel *Horizontal(int r);
AdjRel *Vertical(int r);
AdjRel *Box(int ncols, int nrows);
AdjRel *ShearedBox(int xsize, int ysize, float Si, float Sj);

#endif
