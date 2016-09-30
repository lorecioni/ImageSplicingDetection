#include "libdescriptors.h"

int main(int argc, char** argv)
{
    double *fv1=NULL;
    double *fv2=NULL;
    double distance=0;

    if (argc != 3) {
        fprintf(stderr,"usage: spytec_distance <fv1_path> <fv2_path>\n");
        exit(-1);
    }

    fv1 = ReadFileDouble(argv[1], QTD_COEFS);
    fv2 = ReadFileDouble(argv[2], QTD_COEFS);

    distance=L2DoubleDistance(fv1, fv2, QTD_COEFS);

    printf("%lf\n",distance);

    free(fv1);
    free(fv2);

    return(0);
}
