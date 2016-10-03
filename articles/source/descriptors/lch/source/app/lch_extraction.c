#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  CImage *cimg=NULL;
  Image *mask=NULL;
  Histogram *lch=NULL;

  if (argc != 3) {
    fprintf(stderr,"usage: lch_extraction img_path fv_path\n");
    exit(-1);
  }

  cimg = ReadCImage(argv[1]);
  mask = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

  lch = LCH(cimg, mask);

  WriteFileHistogram(lch,argv[2]);
  DestroyHistogram(&lch);
  DestroyCImage(&cimg);
  DestroyImage(&mask);

  return(0);
}
