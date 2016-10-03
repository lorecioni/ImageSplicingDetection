#ifndef _SPYTEC_H_
#define _SPYTEC_H_

#include "image.h"
#include "cimage.h"
#include "common.h"
#include "dimage.h"

/*converte de radianos para graus*/
#ifndef RAD2DEG
#define RAD2DEG(r) (180/M_PI)*r
#endif

/*qtde de coeficientes considerados para gerar o vetor*/
#define QTD_COEFS 16

/*threshold para as amplitudes das bordas (10% do valor maximo=255)*/
#define TS_G 25

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

typedef struct _imageEdges {
  float G,Ang;
} ImageEdges;

/*Funcoes auxiliares*/
ImageHSV **CreateImageHSV(int ncols, int nrows);
void DestroyImageHSV(ImageHSV **img, int nrows);
ImageEdges **CreateImageEdges(int ncols, int nrows);
void DestroyImageEdges(ImageEdges **img, int nrows);
void RGB2HSV_spytec(CImage *RGB, ImageHSV **HSV);
void Sobel(Image *img, ImageEdges **imgEdges);
void HaarWaveletDecomposition(Image *img, DImage *imgResHH, DImage *imgResHL, DImage *imgResLH, DImage *imgResLL);
void Normalize(DImage *img);
void NormalizeMax(DImage *img);
void NormalizeMaxHH(DImage *img, double max);

double *OrdenaCoeficientes(DImage *imgHH, DImage *imgHL, DImage *imgLH, DImage *imgLL);

double *ReadFileDouble(char *filename, int size);
void WriteFileDouble(char *filename, double *vet, int size);

double L2DoubleDistance(double *v1, double *v2, int size);

#endif
