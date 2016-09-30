#ifndef _FEATUREVECTOR_H_
#define _FEATUREVECTOR_H_

#include "common.h"

/*data structure used to compute distances */
typedef struct _FeatureVector1D {
  double *X;
  int n; 
} FeatureVector1D;

typedef struct _FeatureVector2D {
  double *X;
  double *Y;
  int n; 
} FeatureVector2D;

typedef FeatureVector1D *Ap_FeatureVector1D;
typedef FeatureVector2D *Ap_FeatureVector2D;

FeatureVector1D     *CreateFeatureVector1D(int n);
void                 DestroyFeatureVector1D(FeatureVector1D **fv);
Ap_FeatureVector1D  *AllocAp_FeatureVector1DArray(int n);
FeatureVector1D     *ReadStreamFeatureVector1D(FILE *fp);
FeatureVector1D     *ReadFileFeatureVector1D(char *filename);
Ap_FeatureVector1D  *ReadAp_FeatureVector1D(char *filename, int *n);
void                 WriteStreamFeatureVector1D(FeatureVector1D *fv,FILE *fp);
void                 WriteFileFeatureVector1D(FeatureVector1D *fv,char *filename);
void                 WriteAp_FeatureVector1D(Ap_FeatureVector1D *ap_fv,int n,char *filename);
FeatureVector1D     *CopyFeatureVector1D(FeatureVector1D *fv);
void                 SortFeatureVector1D(FeatureVector1D *fv, int left, int right, char order);
int                  PartFeatureVector1D(FeatureVector1D *fv, int left, int right, char order);

FeatureVector2D     *CreateFeatureVector2D(int n);
void                 DestroyFeatureVector2D(FeatureVector2D **fv);
Ap_FeatureVector2D  *AllocAp_FeatureVector2DArray(int n);
FeatureVector2D     *ReadStreamFeatureVector2D(FILE *fp);
FeatureVector2D     *ReadFileFeatureVector2D(char *filename);
Ap_FeatureVector2D  *ReadAp_FeatureVector2D(char *filename,int *n);
void                 WriteStreamFeatureVector2D(FeatureVector2D *fv,FILE *fp);
void                 WriteFileFeatureVector2D(FeatureVector2D *fv,char *filename);
void                 WriteAp_FeatureVector2D(Ap_FeatureVector2D *ap_fv,int n,char *filename);
FeatureVector2D     *CopyFeatureVector2D(FeatureVector2D *fv);
void                 SortFeatureVector2D(FeatureVector2D *fv, int left, int right, char order);
int                  PartFeatureVector2D(FeatureVector2D *fv, int left, int right, char order);
void                 FeatureVector2DInvertXY(FeatureVector2D *fv);

#endif

