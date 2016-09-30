#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *lch=NULL;

  cimg = ReadCImage(img_path);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  lch = LCH(cimg, mask);

  WriteFileHistogram(lch,fv_path);
  DestroyHistogram(&lch);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

}

double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *lch1=NULL;
  Histogram *lch2=NULL;
  double distance;

  lch1 = ReadFileHistogram(fv1_path);
  lch2 = ReadFileHistogram(fv2_path);

  distance= (double) L1Distance(lch1, lch2);

  DestroyHistogram(&lch1);
  DestroyHistogram(&lch2);

  return distance;
}
