#ifndef _CANNY_H_
#define _CANNY_H_

#include "common.h"
#include "cimage.h"
#include "image.h"
#include "cimage.h"
#include "spytec.h"


#define TS_G 25

/*converte de radianos para graus*/
#ifndef RAD2DEG
#define RAD2DEG(r) (180/M_PI)*r
#endif

/*
typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

typedef struct _imageEdges {
  float G,Ang;
} ImageEdges;
*/

/*Funcoes auxiliares*/
void Smooth(Image *img, Image *imgSuave);
void RGB2HSV_canny(CImage *RGB, ImageHSV **HSV);
void Sobel(Image *img, ImageEdges **imgEdges);
void NormDirection(ImageEdges **imgEdges, int nrows, int ncols);
void NonMaxSuppression(ImageEdges **imgEdges, Image *img);
Image *Hysteresis(Image *img, int T1, int T2);
Image *Canny(CImage *cimg, int T1, int T2);

#endif
