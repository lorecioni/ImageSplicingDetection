#include "cimage.h"

#include "adjacency.h"
#include "color.h" 
#include "mathematics.h"

CImage *CreateCImage(int ncols, int nrows)
{
  CImage *cimg=NULL;
  int i;

  cimg = (CImage *) calloc(1, sizeof(CImage));
  for (i=0; i < 3; i++) 
    cimg->C[i] = CreateImage(ncols,nrows);
  return(cimg);
}

void    DestroyCImage(CImage **cimg)
{
  CImage *tmp;
  int i;

  tmp = *cimg;
  if (tmp != NULL) {
    for (i=0; i < 3; i++) 
      DestroyImage(&(tmp->C[i]));
    free(tmp);
    *cimg = NULL;
  }
}

CImage *ReadCImage(char *filename)
{
  CImage *cimg=NULL;
  FILE *fp=NULL;
  char type[10];
  int  i,ncols,nrows,n;
  char z[256];

  fp = fopen(filename,"rb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%s\n",type);
  if((strcmp(type,"P6")==0)){
    NCFgets(z,255,fp);
    sscanf(z,"%d %d\n",&ncols,&nrows);
    n = ncols*nrows;
    NCFgets(z,255,fp);
    sscanf(z,"%d\n",&i);
    cimg = CreateCImage(ncols,nrows);
    for (i=0; i < n; i++){
      cimg->C[0]->val[i] = fgetc(fp);
      cimg->C[1]->val[i] = fgetc(fp);
      cimg->C[2]->val[i] = fgetc(fp);
    }
    fclose(fp);
  }else{
    fprintf(stderr,"Input image must be P6\n");
    exit(-1);
  }

  return(cimg);
}

void    WriteCImage(CImage *cimg, char *filename)
{
  FILE *fp;
  int i,n;

  fp = fopen(filename,"w");
  fprintf(fp,"P6\n");
  fprintf(fp,"%d %d\n",cimg->C[0]->ncols,cimg->C[0]->nrows);
  fprintf(fp,"255\n"); 
  n = cimg->C[0]->ncols*cimg->C[0]->nrows;
  for (i=0; i < n; i++) {
    fputc(cimg->C[0]->val[i],fp);
    fputc(cimg->C[1]->val[i],fp);
    fputc(cimg->C[2]->val[i],fp);
  }
  fclose(fp); 
}

CImage *CopyCImage(CImage *cimg)
{
  CImage *imgc;
  int i;

  imgc = (CImage *) calloc(1,sizeof(CImage));
  if (imgc == NULL){
    Error(MSG1,"CopyCImage");
  }
  for (i=0; i<3; i++)
    imgc->C[i] = CopyImage(cimg->C[i]);
  return imgc;
}

CImage *Convert2CImage(Image *img)
{
  CImage *imgc;
  int i;

  imgc = (CImage *) calloc(1,sizeof(CImage));
  if (imgc == NULL){
    Error(MSG1,"CopyCImage");
  }
  for (i=0; i<3; i++)
    imgc->C[i] = CopyImage(img);
  return imgc;
}

CImage *CROI(CImage *cimg, int xl, int yl, int xr, int yr)
{
  CImage *croi=NULL;
  int i;

  if (ValidPixel(cimg->C[0],xl,yl)&&ValidPixel(cimg->C[0],xr,yr)&&
      (xl <= xr)&&(yl <= yr)) {
    croi = (CImage *) calloc(1,sizeof(CImage));
    if (croi == NULL){
      Error(MSG1,"CreateCImage");
    }
    for (i=0; i<3; i++)
      croi->C[i] = ROI(cimg->C[i], xl, yl, xr, yr);
  }
  return (croi);
}

CImage *CScale(CImage *cimg, float Sx, float Sy)
{
  CImage *scl=NULL;
  int i;

  scl = (CImage *) calloc(1,sizeof(CImage));
  if (scl == NULL){
    Error(MSG1,"CreateCImage");
  }
  for (i=0; i<3; i++)
    scl->C[i] = Scale(cimg->C[i], Sx, Sy);
  return (scl);
}


CImage *COverlayAlphaImage (CImage *cimg, Image *alpha)
{
  CImage *out=NULL;
  int i;

  out = (CImage *) calloc(1,sizeof(CImage));
  if (out == NULL){
    Error(MSG1,"CreateCImage");
  }
  for (i=0; i<3; i++)
    out->C[i] = OverlayAlphaImage(cimg->C[i], alpha);
  return (out);
}


Image  *GetBand(CImage *cimg, char band)
{
  Image *img=CopyImage(cimg->C[(int)band]);

  return(img);
}

CImage *ColorizeImage(Image *img, float R, float G, float B)
{
  CImage *cimg=CreateCImage(img->ncols,img->nrows);
  float Cb,Cr,Y;
  int Imax,p,n=img->ncols*img->nrows;
 
  Imax = MAX(MaximumValue(img),1);

  Cb  = -0.148*R-0.291*G+0.439*B+128.0/255.;
  Cr  = 0.439*R-0.368*G-0.071*B+128.0/255.;
  
  for (p=0; p < n; p++) {

    Y = ((float)img->val[p]/Imax);

    R=296.82*(Y-0.062745098039)+
      406.98*(Cr-0.50196078431);
      
    G=296.82*(Y-0.062745098039)-
      207.315*(Cr-0.50196078431)-
      99.96*(Cb-0.50196078431);
      
    B=296.82*(Y-0.062745098039)+
      514.335*(Cb-0.50196078431);

    if (R<0.0) R=0.0;
    if (G<0.0) G=0.0;
    if (B<0.0) B=0.0;
    if (R>255.0) R=255.0;
    if (G>255.0) G=255.0;
    if (B>255.0) B=255.0;
    cimg->C[0]->val[p]=(int)(R);
    cimg->C[1]->val[p]=(int)(G);
    cimg->C[2]->val[p]=(int)(B);
  }
  return(cimg);
}


CImage *BlendImages(CImage *cimg1, CImage *cimg2, float alpha1, float alpha2)
{
  CImage *cimg3=CreateCImage(cimg1->C[0]->ncols,cimg1->C[0]->nrows);
  int p,n=cimg1->C[0]->ncols*cimg1->C[0]->nrows;

  for (p=0; p < n; p++) {
    cimg3->C[0]->val[p] = alpha1*cimg1->C[0]->val[p] + alpha2*cimg2->C[0]->val[p];
    cimg3->C[1]->val[p] = alpha1*cimg1->C[1]->val[p] + alpha2*cimg2->C[1]->val[p];
    cimg3->C[2]->val[p] = alpha1*cimg1->C[2]->val[p] + alpha2*cimg2->C[2]->val[p];
  }
  return(cimg3);
}

CImage *CLinearStretch(CImage *cimg, int f1, int f2, int g1, int g2)
{
  CImage *simg=NULL;
  int p,n;
  float a,R,G,B,Y,Cb,Cr,yf1,yf2,yg1,yg2;

  simg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = simg->C[0]->ncols*simg->C[0]->nrows;
  if (f1 != f2) {
    a = ((float)(g2-g1))/(f2-f1);    
  }
  else {
    a = INT_MAX;
  }

  yf1 = (float)f1/255.;
  yf2 = (float)f2/255.;
  yg1 = (float)g1/255.;
  yg2 = (float)g2/255.;

  for (p=0; p < n; p++){

    R = (float)cimg->C[0]->val[p]/255.;
    G = (float)cimg->C[1]->val[p]/255.;
    B = (float)cimg->C[2]->val[p]/255.;

    Y   = 0.257*R+0.504*G+0.098*B+16.0/255.;
    Cb  =-0.148*R-0.291*G+0.439*B+128.0/255.;
    Cr  = 0.439*R-0.368*G-0.071*B+128.0/255.;

    if (Y < yf1)
      Y = yg1;
    else 
      if (Y > yf2)
	Y = yg2;
      else {
	if (a != INT_MAX)	  
	  Y = (a*(Y-yf1)+yg1);
	else{
	  Y = yg2;
	}   
      }

    R= 296.82*(Y-0.062745098039)+\
       406.98*(Cr-0.50196078431);
       
    G =296.82*(Y-0.062745098039)-\
       207.315*(Cr-0.50196078431)-\
       99.96*(Cb-0.50196078431);
       
    B= 296.82*(Y-0.062745098039)+\
       514.335*(Cb-0.50196078431);

    if (R<0.0) R=0.0;
    if (G<0.0) G=0.0;
    if (B<0.0) B=0.0;
    if (R>255.0) R=255.0;
    if (G>255.0) G=255.0;
    if (B>255.0) B=255.0;

    simg->C[0]->val[p]=(int)(R);
    simg->C[1]->val[p]=(int)(G);
    simg->C[2]->val[p]=(int)(B);      
  }
  return(simg);
}

CImage *CImageRGBtoYCbCr(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = RGB2YCbCr(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageYCbCrtoRGB(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = YCbCr2RGB(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageRGBtoHSV(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = RGB2HSV(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageHSVtoRGB(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = HSV2RGB(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageRGBtoXYZ(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = RGB2XYZ(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageXYZtoRGB(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = XYZ2RGB(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageXYZtoLAB(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = XYZ2LAB(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *CImageLABtoXYZ(CImage *cimg)
{
  CImage *ncimg=NULL;
  int p,n,i;


  ncimg = CreateCImage(cimg->C[0]->ncols,cimg->C[0]->nrows);
  n    = ncimg->C[0]->ncols*ncimg->C[0]->nrows;

  for (p=0; p < n; p++){

    i = triplet(cimg->C[0]->val[p],cimg->C[1]->val[p],cimg->C[2]->val[p]);
    i = LAB2XYZ(i);
    ncimg->C[0]->val[p]=t0(i);
    ncimg->C[1]->val[p]=t1(i);
    ncimg->C[2]->val[p]=t2(i);
  }

  return(ncimg);
}

CImage *DrawCBorder(Image *img, Image *label){
  CImage *border;
  int i,j,k,p,q,u,v;
  AdjRel *A;
  Image *img8;

  img8 = ConvertToNbits(img, 8);
  border = Convert2CImage(img8);
  DestroyImage(&img8);
  A = Circular(1.0);
  for(i=0;i<img->nrows;i++){
    for(j=0;j<img->ncols;j++){
      p = j + img->tbrow[i];
      for(k=1;k<A->n;k++){
	u = j+A->dx[k];
	v = i+A->dy[k];
	if(ValidPixel(img,u,v)){
	  q= u + img->tbrow[v];
	  if (label->val[p] < label->val[q]){
	    switch( label->val[q]){
	    case 0:
	      border->C[0]->val[p] = 255;
	      border->C[1]->val[p] = 255;
	      border->C[2]->val[p] = 255;	    
	      break;
	    case 1:
	      border->C[0]->val[p] = 255;
	      border->C[1]->val[p] = 0;
	      border->C[2]->val[p] = 0;	      
	      break;
	    case 2:
	      border->C[0]->val[p] = 0;
	      border->C[1]->val[p] = 255;
	      border->C[2]->val[p] = 0;	      
	      break;
	    case 3:
	      border->C[0]->val[p] = 0;
	      border->C[1]->val[p] = 0;
	      border->C[2]->val[p] = 255;
	      break;
	    default:
	      border->C[0]->val[p] = 255;
	      border->C[1]->val[p] = 255;
	      border->C[2]->val[p] = 0;
	    }
	    
	    break;
	  }
	}
      }
    }
  }
  DestroyAdjRel(&A);

  return border;
}

void    DrawCPoint(CImage *cimg, int x, int y, float raio, int R, int G, int B) 
{
  int p,i,j;
  
  for(i=-raio;i<=raio;i++){
    for(j=-raio;j<=raio;j++){
      if ((x+j >= 0) && (x+j < cimg->C[0]->ncols) &&
	  (y+i >= 0) && (y+i < cimg->C[0]->nrows)){
	if(pow(j,2.0)+pow(i,2.0)<=pow(raio,2.0)){
	  p = x+j + cimg->C[0]->tbrow[y+i];
	  cimg->C[0]->val[p] = R;
	  cimg->C[1]->val[p] = G;
	  cimg->C[2]->val[p] = B;
	}
      }
    }
  }
}
