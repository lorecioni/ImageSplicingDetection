#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
  CImage *cimg=NULL;
  Histogram *acc=NULL;

  cimg = ReadCImage(img_path);

  acc = ACC(cimg);

  WriteFileHistogram(acc,fv_path);
  DestroyHistogram(&acc);
  DestroyCImage(&cimg);

}


double Distance(char *fv1_path, char *fv2_path)
{
  Histogram *acc1=NULL;
  Histogram *acc2=NULL;
  double distance;

  acc1 = ReadFileHistogram(fv1_path);
  acc2 = ReadFileHistogram(fv2_path);

  distance = (double) L1Distance(acc1, acc2);

  DestroyHistogram(&acc1);
  DestroyHistogram(&acc2);

  return distance;
}
