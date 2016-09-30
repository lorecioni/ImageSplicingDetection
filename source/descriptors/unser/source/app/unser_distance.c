#include "libdescriptors.h"

int main(int argc, char** argv)
{
    //  FeatureVector1D *unser1=NULL;
    //  FeatureVector1D *unser2=NULL;
    float *unser1=NULL;
    float *unser2=NULL;
    double distance;
    int i;

    if (argc != 3) {
        fprintf(stderr,"usage: unser_distance <curve1> <curve2>\n");
        exit(-1);
    }

    //unser1 = ReadFileFeatureVector1D(argv[1]);
    //unser2 = ReadFileFeatureVector1D(argv[2]);
    unser1 = ReadUnser(argv[1]);
    unser2 = ReadUnser(argv[2]);

    //distance=ManhattanDistance(unser1, unser2);
    distance=0.0;
    for (i=0; i<SIZE; i++) {
        distance += (double) fabsf(unser1[i] - unser2[i]);
    }
    printf("%lf\n",distance);

    //DestroyFeatureVector1D(&unser1);
    //DestroyFeatureVector1D(&unser2);
    free(unser1);
    free(unser2);

    return(0);
}
