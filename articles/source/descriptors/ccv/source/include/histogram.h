#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include "common.h"

typedef struct _Histogram {
  uchar *v;
  int n; 
} Histogram;

typedef Histogram *Ap_Histogram;

Histogram     *CreateHistogram(int n);
void           DestroyHistogram(Histogram **h);
Ap_Histogram  *AllocAp_HistogramArray(int n);
Histogram     *ReadStreamHistogram(FILE *fp);
Histogram     *ReadFileHistogram(char *filename);
Ap_Histogram  *ReadAp_Histogram(char *filename, int *n);
void           WriteStreamHistogram(Histogram *h,FILE *fp);
void           WriteFileHistogram(Histogram *h,char *filename);
void           WriteAp_Histogram(Ap_Histogram *ap_h,int n,char *filename);
Histogram     *CopyHistogram(Histogram *h);

ulong          L1Distance(Histogram *h1, Histogram *h2);
ulong          L2Distance(Histogram *h1, Histogram *h2);
ulong          LMDistance(Histogram *h1, Histogram *h2);

#endif

