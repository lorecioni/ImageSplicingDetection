#ifndef _IRM_H_
#define _IRM_H_

#include "featurevector.h"
#include "metrics.h"

double IRM(Ap_FeatureVector1D *ap_fv1, FeatureVector1D *w1, 
           Ap_FeatureVector1D *ap_fv2, FeatureVector1D *w2,
           Metric m);

#endif
