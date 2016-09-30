#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *bic=NULL;

  if (argc != 3) {
    fprintf(stderr,"usage: bic_extraction img_path fv_path\n");
    exit(-1);
  }

  cimg = ReadCImage(argv[1]);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  bic = BIC(cimg, mask);

  WriteFileHistogram(bic,argv[2]);
  DestroyHistogram(&bic);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

  return(0);
}
