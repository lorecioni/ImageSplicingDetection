#include "unser.h"

//Encontra o minimo valor entre 3
float min(float x, float y, float z) {

    if ( (x<=y) && (x<=z) ) {
        return x;
    } else if ( (y<=x) && (y<=z) ) {
        return y;
    } else if ( (z<=x) && (z<=y) ) {
        return z;
    }
    return -1;
}

//Encontra o maximo valor entre 3
float max(float x, float y, float z) {

    if ( (x>=y) && (x>=z) ) {
        return x;
    } else if ( (y>=x) && (y>=z) ) {
        return y;
    } else if ( (z>=x) && (z>=y) ) {
        return z;
    }
    return -1;
}

void RGB2HSV_unser(CImage* RGB, ImageHSV** HSV) {

    float r, g, b;
    float minVal, maxVal, delta;
    //float del_R, del_G, del_B;
    int i, j;

    for (i = 0; i < RGB->C[0]->nrows; i++) {
        for (j = 0; j < RGB->C[0]->ncols; j++) {

            //normaliza de 0 a 1
            r = (float) RGB->C[0]->val[j+RGB->C[0]->tbrow[i]]/255;
            g = (float) RGB->C[1]->val[j+RGB->C[1]->tbrow[i]]/255;
            b = (float) RGB->C[2]->val[j+RGB->C[2]->tbrow[i]]/255;

            minVal = min(r, g, b);
            maxVal = max(r, g, b);
            delta = maxVal - minVal;

            //H
            if (delta == 0) {
                HSV[i][j].H = 0;
            } else if (maxVal==r && g>=b) {
                HSV[i][j].H = 60*((g-b)/delta);
            } else if (maxVal==r && g<b) {
                HSV[i][j].H = 60*((g-b)/delta) + 360;
            } else if (maxVal==g) {
                HSV[i][j].H = 60*((b-r)/delta) + 120;
            } else if (maxVal==b) {
                HSV[i][j].H = 60* ((r-g)/delta) + 240;
            }

            //S
            if (maxVal==0) {
                HSV[i][j].S = 0;
            } else {
                HSV[i][j].S = delta/maxVal;
            }

            //V
            HSV[i][j].V = maxVal;

            //normalizando S e V entre 0 e 255
            HSV[i][j].S *= 255;
            HSV[i][j].V *= 255;

            //colocando H de 0 a 1
            //HSV[i][j].H /= 360;

        }
    }
}

void ComputeHistograms(Image *img, float sum[4][511], float dif[4][511])
{
  ulong x, y, p, q;
  ulong npixels;
  AdjRel *A;
  int i, j;
  Pixel v;
  
  A = Circular(1.5);
//  printf("A->n=%d\n",A->n);
//  for (i=1; i <= A->n>>1; i++){
//    printf("i=%d\tdist=[%d,%d]\n",i,A->dx[i], A->dy[i]);
//  }

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

FeatureVector1D *Unser(Image *img)
{
  FeatureVector1D *fv = NULL;
  float sum[4][511], dif[4][511], val[SIZE];
  float mean, contrast, correlation;
  int i;

  ComputeHistograms(img, sum, dif);

  for (i=0; i<4; i++){
    mean = Mean(sum[i]);
    val[i * 8 + 0] = mean;
    contrast = Contrast(dif[i]);
    val[i * 8 + 1] = contrast;
    correlation = Correlation(sum[i], mean, contrast);
    val[i * 8 + 2] = correlation;
    val[i * 8 + 3] = Energy(sum[i], dif[i]);
    val[i * 8 + 4] = Entropy(sum[i], dif[i]);
    val[i * 8 + 5] = Homogeneity(dif[i]);
    val[i * 8 + 6] = MaximalProbability(sum[i]);
    val[i * 8 + 7] = StandardDeviation(contrast, correlation);
  }

  fv = CreateFeatureVector1D(SIZE);
  for (i=0; i<SIZE; i++)
    fv->X[i] = val[i];

  return(fv);
}

float *Unser_float(Image *img)
{
  //Histogram *h = NULL;
  float sum[4][511], dif[4][511], *val=NULL;
  float mean, contrast, correlation;
  int i;

  val = (float*) calloc(SIZE,sizeof(float));

  ComputeHistograms(img, sum, dif);

  for (i=0; i<4; i++){
    mean = Mean(sum[i]);
    val[i * 8 + 0] = mean;
    contrast = Contrast(dif[i]);
    val[i * 8 + 1] = contrast;// / 255.0;
    correlation = Correlation(sum[i], mean, contrast);
    val[i * 8 + 2] = (correlation);// + 32512.5) / 255.0;
    val[i * 8 + 3] = Energy(sum[i], dif[i]);// * 255.0 ;
    val[i * 8 + 4] = Entropy(sum[i], dif[i]);// * 255.0 / 5.4168418;
    val[i * 8 + 5] = Homogeneity(dif[i]);// * 255.0;
    val[i * 8 + 6] = MaximalProbability(sum[i]);// * 255.0;
    val[i * 8 + 7] = StandardDeviation(contrast, correlation);// *sqrt(2);
  }

  //h = CreateHistogram(SIZE);
  //LinearNormalizeHistogram(h->v, val, 255.0, SIZE);

  //return(h);
  return(val);
}

float *ReadUnser(char *filename) {
    FILE *f;
    float *unser=NULL;

    unser = (float*) calloc(SIZE, sizeof(float));

    if ((f = fopen(filename, "rb")) == NULL) {
        printf("erro abrindo arquivo: %s\n", filename);
        exit(0);
    }
    fread(unser, sizeof(float), SIZE, f);
    fclose(f);
    return unser;
}