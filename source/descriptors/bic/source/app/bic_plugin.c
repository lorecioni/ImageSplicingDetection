#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *bic=NULL;

  cimg = ReadCImage(img_path);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  bic = BIC(cimg, mask);

  WriteFileHistogram(bic,fv_path);
  DestroyHistogram(&bic);
  DestroyImage(&mask);
  DestroyCImage(&cimg);

}




double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *bic1=NULL;
  Histogram *bic2=NULL;
  double distance;

  bic1 = ReadFileHistogram(fv1_path);
  bic2 = ReadFileHistogram(fv2_path);

  distance = (double) L1Distance(bic1, bic2);

  DestroyHistogram(&bic1);
  DestroyHistogram(&bic2);

  return distance;
}
