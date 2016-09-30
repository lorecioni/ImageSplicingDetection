#ifndef _EMD_H_
#define _EMD_H_
/*
    emd.h

    Last update: 3/24/98

    An implementation of the Earth Movers Distance.
    Based of the solution for the Transportation problem as described in
    "Introduction to Mathematical Programming" by F. S. Hillier and 
    G. J. Lieberman, McGraw-Hill, 1990.

    Copyright (C) 1998 Yossi Rubner
    Computer Science Department, Stanford University
    E-Mail: rubner@cs.stanford.edu   URL: http://vision.stanford.edu/~rubner
*/

#include "featurevector.h"
#include "metrics.h"

double EMD(Ap_FeatureVector1D *ap_fv1, FeatureVector1D *w1, 
           Ap_FeatureVector1D *ap_fv2, FeatureVector1D *w2,
           Metric m);

#endif
