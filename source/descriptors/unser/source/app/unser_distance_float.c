#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
    //Histogram *unser1=NULL;
    //Histogram *unser2=NULL;
    float *unser1=NULL;
    float *unser2=NULL;
    double distance;
    int i;

    if (argc != 3) {
        fprintf(stderr,"usage: unser_distance <fv1_path> <fv2_path>\n");
        exit(-1);
    }

    //unser1 = ReadFileHistogram(argv[1]);
    //unser2 = ReadFileHistogram(argv[2]);
    unser1 = ReadUnser(argv[1]);
    unser2 = ReadUnser(argv[2]);

    //distance=L1Distance(unser1, unser2);
    distance=0.0;
    for (i=0; i<SIZE; i++) {
        distance += (double) fabsf(unser1[i] - unser2[i]);
    }
    printf("%lf\n",distance);

    //DestroyHistogram(&unser1);
    //DestroyHistogram(&unser2);
    free(unser1);
    free(unser2);

    return(0);
}
