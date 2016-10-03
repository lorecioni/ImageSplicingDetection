#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *ccv=NULL;

  if (argc != 3) {
    fprintf(stderr,"usage: ccv_extraction img_path fv_path\n");
    exit(-1);
  }

  cimg = ReadCImage(argv[1]);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  ccv = CCV(cimg, mask);

  WriteFileHistogram(ccv,argv[2]);
  DestroyHistogram(&ccv);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

  return(0);
}
