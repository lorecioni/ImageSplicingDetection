#ifndef _CBC_H_
#define _CBC_H_

#include "cimage.h"
#include "featurevector.h"

Ap_FeatureVector1D *CBC(CImage *cimg, Image *mask, int *nfeatures);

double CBCRegionDistance(FeatureVector1D *fv1, FeatureVector1D *fv2);

#endif
