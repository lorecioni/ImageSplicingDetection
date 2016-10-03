#ifndef _METRICS_H_
#define _METRICS_H_

#include "featurevector.h"

typedef double (* Metric)(FeatureVector1D *, FeatureVector1D *);

double ManhattanDistance(FeatureVector1D *v1, FeatureVector1D *v2);
double EuclideanDistance(FeatureVector1D *v1, FeatureVector1D *v2);
double ChebyshevDistance(FeatureVector1D *v1, FeatureVector1D *v2);

#endif



