#include "unser.h"

#include "common.h"
#include "adjacency.h"

#define SIZE 32

void ComputeHistograms(Image *img, float sum[4][511], float dif[4][511])
{
  ulong x, y, p, q;
  ulong npixels;
  AdjRel *A;
  int i, j;
  Pixel v;
  
  A = Circular(1.5);

  for (i=0; i<4; i++)
    for (j=0; j<=510; j++){
      sum[i][j] = 0.0;
      dif[i][j] = 0.0;
    }

  for(y=1; y<img->nrows-1; y++){
    for(x=1; x<img->ncols-1; x++){
      p = x + img->tbrow[y];
      for (i=1; i <= A->n>>1; i++){
        v.x = x + A->dx[i];
        v.y = y + A->dy[i];
        q = v.x + img->tbrow[v.y];
        if (ValidPixel(img,v.x,v.y)){
          sum[i - 1][img->val[p] + img->val[q]] += 1.0;
          dif[i - 1][img->val[p] - img->val[q] + 255] += 1.0;
        }
      }
    }
  }
  DestroyAdjRel(&A);

  npixels = img->ncols * img->nrows;
  for (i=0; i<4; i++)
    for (j=0; j<=510; j++){
      sum[i][j] /= (float) npixels;
      dif[i][j] /= (float) npixels;
    }
}

float Mean(float s[511])
{
  float mean;
  int i;

  mean = 0.0;
  for (i=0; i<=510; i++)
    mean += i * s[i];
  mean *= 0.5;

  return(mean);
}

float Contrast(float d[511])
{
  float contrast;
  int j;

  contrast = 0.0;
  for (j=-255; j<=255; j++)
    contrast += j * j * d[j+255];

  return(contrast);
}

float Correlation(float s[511], float mean, float contrast)
{
  float correlation, aux;
  int i;

  correlation = 0.0;
  for (i=0; i<=510; i++){
    aux = i - 2.0 * mean;
    correlation += aux * aux * s[i];
  }
  correlation -= contrast;
  correlation *= 0.5;

  return(correlation);
}

float Energy(float s[511], float d[511])
{
  float energy_s, energy_d;
  int i;

  energy_s = 0.0;
  energy_d = 0.0;
  for (i=0; i<=510; i++){
    energy_s += s[i] * s[i];
    energy_d += d[i] * d[i];
  }

  return(energy_s * energy_d);
}

float Entropy(float s[511], float d[511])
{
  float entropy_s, entropy_d;
  int i;

  entropy_s = 0.0;
  entropy_d = 0.0;
  for (i=0; i<=510; i++){
    if (s[i] > 0.0)
      entropy_s += s[i] * log10(s[i]);
    if (d[i] > 0.0)
      entropy_d += d[i] * log10(d[i]);
  }

  return(- entropy_s - entropy_d);
}

float Homogeneity(float d[511])
{
  float homogeneity;
  int j;

  homogeneity = 0.0;
  for (j=-255; j<=255; j++)
    homogeneity += (1.0 / (1.0 + (float)(j * j))) * d[j+255];

  return(homogeneity);
}

float MaximalProbability(float s[511])
{
  float max;
  int i;

  max = 0.0;
  for (i=0; i<=510; i++)
    if (max < s[i])
      max = s[i];

  return(max);
}

float StandardDeviation(float contrast, float correlation)
{
  return(sqrt(correlation+contrast));
}

uchar ComputeNorm(float value)
{
  return ((uchar)(255. * value));
}

void LinearNormalizeHistogram(uchar *ch, float *h, float max, int size)
{
  int i;
  uchar v;
  
  for(i=0; i<size; i++){
    v = ComputeNorm((float) h[i] / (float) max);
    ch[i] = (uchar)(v);
  }
}

uchar ComputeLog(float value)
{
  uchar result;
  
  value = 255. * value;
  if(value==0.)       result=0;
  else if(value<1.)   result=1;
  else if(value<2.)   result=2;
  else if(value<4.)   result=3;
  else if(value<8.)   result=4;
  else if(value<16.)  result=5;
  else if(value<32.)  result=6;
  else if(value<64.)  result=7;
  else if(value<128.) result=8;
  else                result=9;
  
  return(result);
}

void NonLinearNormalizeHistogram(uchar *ch, float *h, float max, int size)
{
  int i;
  uchar v;
  
  for(i=0; i<size; i++){
    v = ComputeLog((float) h[i] / (float) max);
    ch[i] = (uchar)(48 + v);
  }
}

Histogram *Unser(Image *img)
{
  Histogram *h = NULL;
  float sum[4][511], dif[4][511], val[SIZE];
  float mean, contrast, correlation;
  int i;

  ComputeHistograms(img, sum, dif);

  for (i=0; i<4; i++){
    mean = Mean(sum[i]);
    val[i * 8 + 0] = mean;
    contrast = Contrast(dif[i]);
    val[i * 8 + 1] = contrast / 255.0;
    correlation = Correlation(sum[i], mean, contrast);
    val[i * 8 + 2] = (correlation + 32512.5) / 255.0;
    val[i * 8 + 3] = 255.0 * Energy(sum[i], dif[i]);
    val[i * 8 + 4] = 255.0 * Entropy(sum[i], dif[i]) / 5.4168418;
    val[i * 8 + 5] = 255.0 * Homogeneity(dif[i]);
    val[i * 8 + 6] = 255.0 * MaximalProbability(sum[i]);
    val[i * 8 + 7] = sqrt(2)*StandardDeviation(contrast, correlation);
  }

  h = CreateHistogram(SIZE);
  LinearNormalizeHistogram(h->v, val, 255.0, SIZE);

  return(h);
}

