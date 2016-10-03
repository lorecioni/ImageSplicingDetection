#include "metrics.h"

double ManhattanDistance(FeatureVector1D *v1, FeatureVector1D *v2) { 
  int i;
  double sum = 0.0;
  double z = 0.0;
  
  for (i = 0; i < v1->n ; i++){
    z = fabs(v1->X[i] - v2->X[i]); 
    sum = sum + z;
  }
  return (sum);
}

double EuclideanDistance(FeatureVector1D *v1, FeatureVector1D *v2) { 
  int i;
  double sum = 0.0;
  double z = 0.0;
  
  for (i = 0; i < v1->n ; i++){
    z = v1->X[i] - v2->X[i]; 
    sum += z*z;
  }
  sum = sqrt(sum);
  return (sum);
}

double ChebyshevDistance(FeatureVector1D *v1, FeatureVector1D *v2) { 
  int i;
  double max = -1.0;
  double z = 0.0;
  
  for (i = 0; i < v1->n ; i++){
    z = fabs(v1->X[i] - v2->X[i]); 
    if (max < z) max = z;
  }
  return (max);
}

