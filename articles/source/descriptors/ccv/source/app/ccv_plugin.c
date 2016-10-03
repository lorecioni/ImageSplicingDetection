#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *ccv=NULL;

  cimg = ReadCImage(img_path);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  ccv = CCV(cimg, mask);

  WriteFileHistogram(ccv,fv_path);
  DestroyHistogram(&ccv);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

}

double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *ccv1=NULL;
  Histogram *ccv2=NULL;
  double distance;

  ccv1 = ReadFileHistogram(fv1_path);
  ccv2 = ReadFileHistogram(fv2_path);
  
  distance = (double) L1Distance(ccv1, ccv2);

  DestroyHistogram(&ccv1);
  DestroyHistogram(&ccv2);

  return distance;
}
