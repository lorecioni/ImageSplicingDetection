#ifndef _DIMAGE_H_
#define _DIMAGE_H_

#include "common.h"

typedef struct _DImage{
  double *val;
  int ncols,nrows;
  int *tbrow;
} DImage;

DImage    *CreateDImage(int ncols, int nrows);
void       DestroyDImage(DImage **dimg);

#endif
