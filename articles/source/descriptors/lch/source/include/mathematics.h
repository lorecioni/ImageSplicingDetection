#ifndef _MATHEMATICS_H_
#define _MATHEMATICS_H_

#include "image.h"

Image  *Diff(Image *img1,Image *img2); 
Image  *Sum(Image *img1, Image *img2);
Image  *SQRT(Image *img);
Image  *And(Image *img1, Image *img2);
Image  *Or(Image *img1, Image *img2);
Image  *Add(Image *img, int value);
Image  *Sub(Image *img, int value);
Image  *Complement(Image *img);
Image  *Div(Image *img1,Image *img2);
Image  *Mult(Image *img1,Image *img2);
Image  *Abs(Image *img);
Image  *Average(Image *img1, Image *img2);
Image  *Log(Image *img);

#endif


