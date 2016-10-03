#ifndef _SASI_H_
#define _SASI_H_

#include "image.h"
#include "featurevector.h"
#include "cimage.h"
#include "common.h"

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;


FeatureVector1D *SASI(Image *img, int num, int *sizes);

void RGB2HSV_sasi(CImage *RGB, ImageHSV **HSV);
FeatureVector1D *ReadFileFeatureVector1DBin(char *filename);

#endif
