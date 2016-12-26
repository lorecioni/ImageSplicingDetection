#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *gch=NULL;

  cimg = ReadCImage(img_path);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  gch = GCH(cimg, mask);

  WriteFileHistogram(gch,fv_path);
  DestroyHistogram(&gch);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

}


double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *gch1=NULL;
  Histogram *gch2=NULL;
  double distance;

  gch1 = ReadFileHistogram(fv1_path);
  gch2 = ReadFileHistogram(fv2_path);

  distance = (double) L1Distance(gch1, gch2);

  DestroyHistogram(&gch1);
  DestroyHistogram(&gch2);

  return distance;
}
