#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *lch1=NULL;
  Histogram *lch2=NULL;
  double distance;

  if (argc != 3) {
    fprintf(stderr,"usage: lch_distance fv1_path fv2_path\n");
    exit(-1);
  }

  lch1 = ReadFileHistogram(argv[1]);
  lch2 = ReadFileHistogram(argv[2]);

  distance=(double)L1Distance(lch1, lch2);
  printf("%lf\n",distance);

  DestroyHistogram(&lch1);
  DestroyHistogram(&lch2);

  return(0);
}
