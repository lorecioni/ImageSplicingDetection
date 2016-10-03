#include "irm.h"

#include "common.h"
#include "sort.h"

typedef struct _Pair {
  int i, j;
  double d;
} Pair;

int ComparePair(const void *a, const void *b) {
  double d;

  d =  (*(Pair *)a).d - (*(Pair *)b).d;
  if (d < 0)
    return -1;
  else if (d > 0)
    return  1;
  else
    return  0;
}

double IRM(Ap_FeatureVector1D *ap_fv1, FeatureVector1D *w1, 
           Ap_FeatureVector1D *ap_fv2, FeatureVector1D *w2,
           Metric m)
{
  double *weight1, *weight2;
  uchar *status1, *status2;
  double w, beta;
  Pair *pairs;
  int n1, n2;
  int i, j;
  int p, n;

  n1 = w1->n;
  n2 = w2->n;

  status1 = AllocUCharArray(n1);
  weight1 = AllocDoubleArray(n1);
  for (i=0; i<n1; i++) {
    status1[i] = 0;
    weight1[i] = w1->X[i];
  }

  status2 = AllocUCharArray(n2);
  weight2 = AllocDoubleArray(n2);
  for (j=0; j<n2; j++) {
    status2[j] = 0;
    weight2[j] = w2->X[j];
  }

  n     = n1 * n2;
  pairs = (Pair *)calloc(n, sizeof(Pair));
  if (pairs == NULL) {
    Error(MSG1,"CreatePairs");
  }

  for (i=0; i<n1; i++)
    for (j=0; j<n2; j++) {
      p = i * n2 + j;
      pairs[p].i = i;
      pairs[p].j = j;
      pairs[p].d = m(ap_fv1[i], ap_fv2[j]);
    }

  QuickSort(pairs, n, sizeof(Pair), ComparePair);

  beta = 0.0;
  for (p=0; p<n; p++) {
    i = pairs[p].i;
    j = pairs[p].j;
    if (status1[i] == 0 && status2[j] == 0) {
      if (weight1[i] < weight2[j]) {
        w = weight1[i];
        weight2[j] -= weight1[i];
        status1[i] = 1;
      }
      else {
        w = weight2[j];
        weight1[i] -= weight2[j];
        status2[j] = 1;
        if (weight1[i] < Epsilon)
          status1[i] = 1;
      }
      beta += w * pairs[p].d;
    }
  }
  free(status1);
  free(status2);
  free(weight1);
  free(weight2);
  free(pairs);

  return(beta);
}

