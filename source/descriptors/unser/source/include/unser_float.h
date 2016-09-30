#ifndef _UNSER_H_
#define _UNSER_H_

#include "image.h"
#include "histogram.h"

#define SIZE 32

//Histogram *Unser(Image *img); //corresponde ao unser_original.c
float *Unser(Image *img);
float *ReadUnser(char *filename);

#endif
