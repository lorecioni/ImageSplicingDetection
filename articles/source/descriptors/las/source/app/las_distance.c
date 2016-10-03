#include "libdescriptors.h"

int main(int argc, char** argv)
{
    float *las1=NULL;
    float *las2=NULL;
    double distance;

    if (argc != 3) {
        fprintf(stderr,"usage: las_distance <fv1_path> <fv2_path>\n");
        exit(-1);
    }

    las1 = ReadFileFloat(argv[1], QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);
    las2 = ReadFileFloat(argv[2], QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);

    distance=L1FloatDistance(las1, las2, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);

    printf("%lf\n",distance);

    free(las1);
    free(las2);

    return(0);
}
