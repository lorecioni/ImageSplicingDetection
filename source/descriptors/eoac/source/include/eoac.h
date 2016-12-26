#ifndef _EOAC_H_
#define _EOAC_H_

#include "common.h"
#include "cimage.h"
#include "image.h"
#include "adjacency.h"

/*constantes para o autocorrelograma*/
#define QTD_DIST 4
#define ANG_INTERVAL 5
#define QTD_ANG (360/ANG_INTERVAL)
/*72 pois sao 360 graus divididos em intervalos de 5 graus (360/5=72)*/

/*threshold para as amplitudes das bordas (10% do valor maximo=255)*/
#define TS_G 25

/*thresholds para indicar se os angulos/amplitudes sao semelhantes ou nao*/
#define TS_ANG 1
#define TS_AMP 5

/*converte de radianos para graus*/
#ifndef RAD2DEG
#define RAD2DEG(r) (180/M_PI)*r
#endif

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

typedef struct _imageEdges {
  float G,Ang;
} ImageEdges;


/*Funcoes auxiliares*/
void CalculaAutocorrelograma(int nrows, int ncols, ImageEdges **img, int *distances, int **eoac);
void RGB2HSV_eoac(CImage *RGB, ImageHSV **HSV);
void Sobel(Image *img, ImageEdges **imgEdges);
float *ReadEOAC(char *filename);

#endif
