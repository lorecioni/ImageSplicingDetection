#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  CImage *cimg=NULL;
  Histogram *acc=NULL;

  if (argc != 3) {
    fprintf(stderr,"usage: acc_extraction <img_path> <fv_path>\n");
    exit(-1);
  }

  cimg = ReadCImage(argv[1]);

  acc = ACC(cimg);

  WriteFileHistogram(acc,argv[2]);
  DestroyHistogram(&acc);
  DestroyCImage(&cimg);

  return(0);
}
