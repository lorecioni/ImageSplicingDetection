#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *acc1=NULL;
  Histogram *acc2=NULL;
  ulong distance;

  if (argc != 3) {
    fprintf(stderr,"usage: acc_distance <fv1_path> <fv2_path>\n");
    exit(-1);
  }

  acc1 = ReadFileHistogram(argv[1]);
  acc2 = ReadFileHistogram(argv[2]);

  distance=L1Distance(acc1, acc2);
  printf("%ld\n",distance);

  DestroyHistogram(&acc1);
  DestroyHistogram(&acc2);

  return(0);
}
