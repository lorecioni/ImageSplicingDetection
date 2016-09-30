#include "libtexturedescriptors.h"

double Distance(char *fv1_path, char *fv2_path)
{
    FeatureVector1D *sasi1=NULL;
    FeatureVector1D *sasi2=NULL;
    double distance;

    sasi1 = ReadFileFeatureVector1DBin(fv1_path);
    sasi2 = ReadFileFeatureVector1DBin(fv2_path);

    distance = DotDistance(sasi1, sasi2); // 1/semelhanca -1 

    DestroyFeatureVector1D(&sasi1);
    DestroyFeatureVector1D(&sasi2);

    return (distance);
}

int main(int argc, char** argv)
{
  double distance;

  if (argc != 3) {
    fprintf(stderr,"usage: gch_distance <fv1_path> <fv2_path>\n");
    exit(-1);
  }

  distance=Distance(argv[1], argv[2]);

  printf("%f\n",distance);

  return(0);
}
