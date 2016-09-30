#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *bic1=NULL;
  Histogram *bic2=NULL;
  double distance;

  if (argc != 3) {
    fprintf(stderr,"usage: bic_distance fv1_path fv2_path\n");
    exit(-1);
  }

  bic1 = ReadFileHistogram(argv[1]);
  bic2 = ReadFileHistogram(argv[2]);

  distance= (double) L1Distance(bic1, bic2);
  printf("%lf\n",distance);

  DestroyHistogram(&bic1);
  DestroyHistogram(&bic2);

  return(0);
}
