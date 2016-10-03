#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *ccv1=NULL;
  Histogram *ccv2=NULL;
  double distance;

  if (argc != 3) {
    fprintf(stderr,"usage: ccv_distance fv1_path fv2_path\n");
    exit(-1);
  }

  ccv1 = ReadFileHistogram(argv[1]);
  ccv2 = ReadFileHistogram(argv[2]);

  distance=(double)L1Distance(ccv1, ccv2);
  printf("%lf\n",distance);

  DestroyHistogram(&ccv1);
  DestroyHistogram(&ccv2);

  return(0);
}
