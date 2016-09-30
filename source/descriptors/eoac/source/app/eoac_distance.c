#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
    float *eoac1=NULL, *eoac2=NULL;
    double distance;
    int i;

    if (argc != 3) {
        fprintf(stderr,"usage: eoac_distance <fv1_path> <fv2_path>\n");
        exit(-1);
    }

    eoac1 = ReadEOAC(argv[1]);
    eoac2 = ReadEOAC(argv[2]);

    distance=0.0;
    for (i=0; i<(QTD_ANG*QTD_DIST); i++) {
        distance += (double) fabsf(eoac1[i] - eoac2[i]);
    }

    printf("%lf\n",distance);

    free(eoac1);
    free(eoac2);

    return distance;
}
