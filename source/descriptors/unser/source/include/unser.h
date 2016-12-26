#ifndef _UNSER_H_
#define _UNSER_H_

#include "image.h"
#include "featurevector.h"
#include "cimage.h"
#include "adjacency.h"
#include "common.h"

#define SIZE 32

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

void RGB2HSV_unser(CImage* RGB, ImageHSV** HSV);
FeatureVector1D *Unser(Image *img);
float *Unser_float(Image *img);

float *ReadUnser(char *filename);

#endif
