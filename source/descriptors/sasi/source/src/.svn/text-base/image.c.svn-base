#include "image.h"

Image *CreateImage(int ncols, int nrows)
{
  Image *img=NULL;
  int i;

  img = (Image *) calloc(1,sizeof(Image));
  if (img == NULL){
    Error(MSG1,"CreateImage");
  }

  img->val   = AllocIntArray(nrows*ncols);
  img->tbrow = AllocIntArray(nrows);

  img->tbrow[0]=0;
  for (i=1; i < nrows; i++)
    img->tbrow[i]=img->tbrow[i-1]+ncols;
  img->ncols = ncols;
  img->nrows = nrows;
 
 return(img);
}

void DestroyImage(Image **img)
{
  Image *aux;

  aux = *img;
  if(aux != NULL){
    if (aux->val != NULL)   free(aux->val); 
    if (aux->tbrow != NULL) free(aux->tbrow);
    free(aux);    
    *img = NULL;
  }
}

Image *ReadImage(char *filename)
{
  FILE *fp=NULL;
  unsigned char *value=NULL;
  char type[10];
  int  i,ncols,nrows,n;
  Image *img=NULL;
  char z[256];

  fp = fopen(filename,"rb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  fscanf(fp,"%s\n",type);
  if((strcmp(type,"P5")==0)){
    NCFgets(z,255,fp);
    sscanf(z,"%d %d\n",&ncols,&nrows);
    n = ncols*nrows;
    NCFgets(z,255,fp);
    sscanf(z,"%d\n",&i);
    value = (unsigned char *)calloc(n,sizeof(unsigned char));
    if (value != NULL){
      fread(value,sizeof(unsigned char),n,fp);
    }else{
      fprintf(stderr,"Insufficient memory in ReadImage\n");
      exit(-1);
    }
    fclose(fp);
    img = CreateImage(ncols,nrows);
    for (i=0; i < n; i++)
      img->val[i]=(int)value[i];
    free(value);
  }else{
    if((strcmp(type,"P2")==0)){
      NCFgets(z,255,fp);
      sscanf(z,"%d %d\n",&ncols,&nrows);
      n = ncols*nrows;
      NCFgets(z,255,fp);
      sscanf(z,"%d\n",&i);
      img = CreateImage(ncols,nrows);
      for (i=0; i < n; i++)
	fscanf(fp,"%d",&img->val[i]);
      fclose(fp);
    }else{
      fprintf(stderr,"Input image must be P2 or P5\n");
      exit(-1);
    }
  }

  return(img);
}

void WriteImage(Image *img,char *filename)
{
  FILE *fp;
  int i, n, Imax;

  fp = fopen(filename,"wb");
  if (fp == NULL){
    fprintf(stderr,"Cannot open %s\n",filename);
    exit(-1);
  }
  n    = img->ncols*img->nrows;
  if ((Imax=MaximumValue(img))==INT_MAX){
    Warning("Image with infinity values","WriteImage");
    Imax = INT_MIN;
    for (i=0; i < n; i++) 
      if ((img->val[i] > Imax)&&(img->val[i]!=INT_MAX))
	Imax = img->val[i];
    fprintf(fp,"P2\n");
    fprintf(fp,"%d %d\n",img->ncols,img->nrows);
    fprintf(fp,"%d\n",Imax+1);
  } else {
    fprintf(fp,"P2\n");
    fprintf(fp,"%d %d\n",img->ncols,img->nrows);
    if (Imax==0) Imax++;
    fprintf(fp,"%d\n",Imax);
  }
 
  for (i=0; i < n; i++) {
    if (img->val[i]==INT_MAX)
      fprintf(fp,"%d ",Imax+1);
    else
      fprintf(fp,"%d ",img->val[i]);
    if (((i+1)%17) == 0)
      fprintf(fp,"\n");
  }

  fclose(fp);

}

Image *ConvertToNbits(Image *img, int N)
{
  Image *imgN;
  int min,max,i,n,Imax;

  imgN = CreateImage(img->ncols,img->nrows);
  n    = img->ncols*img->nrows;
  Imax = (int)(pow(2,N)-1);

  min = INT_MAX;
  max = INT_MIN;
  for (i=0; i < n; i++) {
    if ((img->val[i] != INT_MIN)&&(img->val[i] != INT_MAX)){
      if (img->val[i] > max)
	max = img->val[i];
      if (img->val[i] < min)
	min = img->val[i];
    }
  }
  if (min != max)   
    for (i=0; i < n; i++) {
      if ((img->val[i] != INT_MIN)&&(img->val[i] != INT_MAX)){
	imgN->val[i] = (int)(((float)Imax*(float)(img->val[i] - min))/
	  (float)(max-min)); 
      } else {
	if (img->val[i]==INT_MIN)
	  imgN->val[i] = 0;
	else
	  imgN->val[i] = Imax;
      }
    }
  return(imgN);
}

int MinimumValue(Image *img)
{
  int i,min,n;

  n = img->ncols*img->nrows;
  min = img->val[0];
  for (i=1; i < n; i++)
    if (img->val[i] < min)
      min = img->val[i];

  return(min);
}

int MaximumValue(Image *img)
{
  unsigned int i, n, r;
  int max;

  max = img->val[0];
  n = img->ncols*img->nrows - 1;
  r = n%4;
  n -= r;
  for (i=1; i < n; i+=4) {
    if (img->val[i] > max)
      max = img->val[i];
    if (img->val[i+1] > max)
      max = img->val[i+1];
    if (img->val[i+2] > max)
      max = img->val[i+2];
    if (img->val[i+3] > max)
      max = img->val[i+3];
  }
  while (r != 0) {
    if (img->val[i+r-1] > max)
      max = img->val[i+r-1];
    --r;
  }

  return(max);
}

void SetImage(Image *img, int value)
{ 
  int i,n;
  n = img->ncols*img->nrows;
  for (i=0; i < n; i++){
    img->val[i]=value;
  }
}

bool ValidPixel(Image *img, int x, int y)
{
  if ((x >= 0)&&(x < img->ncols)&&
      (y >= 0)&&(y < img->nrows))
    return(true);
  else
    return(false);
}

Image *CopyImage(Image *img)
{
  Image *imgc;

  imgc = CreateImage(img->ncols,img->nrows);
  memcpy(imgc->val,img->val,img->ncols*img->nrows*sizeof(int));
  
  return(imgc);
}

Image *AddFrame(Image *img, int sz, int value)
{
  Image *fimg;
  int y,*dst,*src,nbytes,offset;

  fimg = CreateImage(img->ncols+(2*sz),img->nrows+(2*sz));
  SetImage(fimg,value);
  nbytes = sizeof(int)*img->ncols;
  offset = sz+fimg->tbrow[sz];
  for (y=0,src=img->val,dst=fimg->val+offset; y < img->nrows;y++,src+=img->ncols,dst+=fimg->ncols){
    memcpy(dst,src,nbytes);
  }
  return(fimg);
}

Image *RemFrame(Image *fimg, int sz)
{
  Image *img;
  int y,*dst,*src,nbytes,offset;

  img = CreateImage(fimg->ncols-(2*sz),fimg->nrows-(2*sz));
  nbytes = sizeof(int)*img->ncols;
  offset = sz+fimg->tbrow[sz];
  for (y=0,src=fimg->val+offset,dst=img->val; y < img->nrows;y++,src+=fimg->ncols,dst+=img->ncols){
    memcpy(dst,src,nbytes);
  }
  return(img);
}


Image *ROI(Image *img, int xl, int yl, int xr, int yr)
{
  int x,y,p,i;
  Image *roi=NULL;
  
  if (ValidPixel(img,xl,yl)&&ValidPixel(img,xr,yr)&&
      (xl <= xr)&&(yl <= yr)        )
    {
      roi = CreateImage(xr-xl+1,yr-yl+1);
      i=0;
      for (y=yl; y <= yr; y++)
	for (x=xl; x <= xr; x++){
	  p = x + img->tbrow[y];
	  roi->val[i] = img->val[p];
	  i++;
	}
    } 
  
  return(roi);	
}

Image *MBB(Image *img)
{
  int x,y;
  Pixel left,right;
  Image *mbb=NULL;
  
  left.x  = img->ncols-1;
  left.y  = img->nrows-1;
  right.x = 0;
  right.y = 0;
  
  for (y=0; y < img->nrows; y++)
    for (x=0; x < img->ncols; x++){
      if (img->val[x+img->tbrow[y]] > 0){
	if (x < left.x)
	  left.x = x;
	if (y < left.y)
	  left.y = y;
	if (x > right.x)
	  right.x = x;
	if (y > right.y)
	  right.y = y;	
      }
    }
  
  mbb = ROI(img,left.x,left.y,right.x,right.y);

  return(mbb);	
}

Image *MakeFrame(int ncols, int nrows, int value)
{
  Image *frame=NULL;
  int x,y;

  frame = CreateImage(ncols,nrows);
  
  for (y=0; y < frame->nrows; y++) {
    frame->val[frame->tbrow[y]] = value;
    frame->val[frame->ncols-1+frame->tbrow[y]] = value;
  }
  for (x=0; x < frame->ncols; x++) {
    frame->val[x+frame->tbrow[0]] = value;
    frame->val[x+frame->tbrow[frame->nrows-1]] = value;
  }

  return(frame);
}

void PaintCircle(Image *img, int center, float radius, int value)
{
  Pixel c,v;
  int r0,r2,dx,dy,p;

  c.x = center%img->ncols;
  c.y = center/img->ncols;
  r0 = (int)radius;
  r2 = (int)(radius*radius);
  for(v.y = (c.y-r0), dy=-r0; v.y <= (c.y+r0); v.y++,dy++)
    for(v.x = (c.x-r0), dx=-r0; v.x <= (c.x+r0); v.x++,dx++)
      if (ValidPixel(img,v.x,v.y)&&(((dx*dx)+(dy*dy)) <= r2)){
	p = v.x + img->tbrow[v.y];
	img->val[p] = value;
      }
}

void DrawPath(Image *img,Image *pred, int dst,int value)
{
  int p;
  
  p = dst;
  while(pred->val[p]!=p){
    img->val[p]=value;
    p = pred->val[p];
  }
  img->val[p]=value;

}

Image *InputSeeds(int ncols, int nrows, char *seeds)
{
  Image *img;
  int n,l;
  Pixel u;

  sscanf(seeds,"%d",&n);
  img = CreateImage(ncols,nrows);
  while (n > 0)  {
    seeds=strchr(seeds,',')+1;
    sscanf(seeds,"%d",&u.x);
    seeds=strchr(seeds,',')+1;
    sscanf(seeds,"%d",&u.y);
    seeds=strchr(seeds,',')+1;
    sscanf(seeds,"%d",&l);
    if (ValidPixel(img,u.x,u.y)){
      img->val[u.x+img->tbrow[u.y]]=l;
    }
    n--;
  }
  return(img);
}

Image *Rotate(Image *img, float theta) 
{
  float R[2][2],x,y,d1,d2,d3,d4,Ix1,Ix2,If;
  Image *rot;
  Pixel u,v,prev,next;
  int diag;

  theta = theta*PI/180.0;
  R[0][0] = cos(theta);
  R[0][1] = sin(theta);
  R[1][0] = -sin(theta);
  R[1][1] = cos(theta);

  diag = sqrt(img->ncols*img->ncols + img->nrows*img->nrows);

  rot = CreateImage(diag,diag); 

  for (v.y=0; v.y < rot->nrows; v.y++)
    for (v.x=0; v.x < rot->ncols; v.x++){
      x = ((v.x-rot->ncols/2.)*R[0][0] + (v.y-rot->nrows/2.)*R[0][1]) 
	+ img->ncols/2.;
      y = ((v.x-rot->ncols/2.)*R[1][0] + (v.y-rot->nrows/2.)*R[1][1]) 
	+ img->nrows/2.;
      u.x = (int)(x+0.5);
      u.y = (int)(y+0.5);
      if (ValidPixel(img,u.x,u.y)){
	if (x < u.x) {
	  next.x = u.x;
	  prev.x = u.x - 1;
	} else {
	  next.x = u.x + 1;
	  prev.x = u.x;
	}
	d1 = next.x - x;
	d2 = x - prev.x;
	if (y < u.y) {
	  next.y = u.y;
	  prev.y = u.y - 1;
	} else {
	  next.y = u.y + 1;
	  prev.y = u.y;
	}
	d3 = next.y - y;
	d4 = y - prev.y;

	if (ValidPixel(img,prev.x,prev.y)&&ValidPixel(img,next.x,prev.y))
	  Ix1 = d1*img->val[prev.x+img->tbrow[prev.y]] + 
	    d2*img->val[next.x+img->tbrow[prev.y]];
	else
	  Ix1 = img->val[u.x+img->tbrow[u.y]];

	if (ValidPixel(img,prev.x,next.y)&&ValidPixel(img,next.x,next.y))
	  Ix2 = d1*img->val[prev.x+img->tbrow[next.y]] + 
	    d2*img->val[next.x+img->tbrow[next.y]];
	else
	  Ix2 = img->val[u.x+img->tbrow[u.y]];
	
	If = d3*Ix1 + d4*Ix2;

	rot->val[v.x+rot->tbrow[v.y]] = (int)If;
      }
    }
  
  return(rot);
}

Image *Scale(Image *img, float Sx, float Sy) 
{
  float S[2][2],x,y,d1,d2,d3,d4,Ix1,Ix2,If;
  Image *scl;
  Pixel u,v,prev,next;
  
  if (Sx == 0.0) Sx = 1.0;
  if (Sy == 0.0) Sy = 1.0;

  S[0][0] = 1.0/Sx;
  S[0][1] = 0;
  S[1][0] = 0;
  S[1][1] = 1.0/Sy;

  scl = CreateImage((int)(img->ncols*Sx + 0.5),(int)(img->nrows*Sy + 0.5)); 

  for (v.y=0; v.y < scl->nrows; v.y++)
    for (v.x=0; v.x < scl->ncols; v.x++){
      x = ((v.x-scl->ncols/2.)*S[0][0] + (v.y-scl->nrows/2.)*S[0][1]) 
	+ img->ncols/2.;
      y = ((v.x-scl->ncols/2.)*S[1][0] + (v.y-scl->nrows/2.)*S[1][1]) 
	+ img->nrows/2.;
      u.x = (int)(x+0.5);
      u.y = (int)(y+0.5);
      if (ValidPixel(img,u.x,u.y)){
	if (x < u.x) {
	  next.x = u.x;
	  prev.x = u.x - 1;
	} else {
	  next.x = u.x + 1;
	  prev.x = u.x;
	}
	d1 = next.x - x;
	d2 = x - prev.x;
	if (y < u.y) {
	  next.y = u.y;
	  prev.y = u.y - 1;
	} else {
	  next.y = u.y + 1;
	  prev.y = u.y;
	}
	d3 = next.y - y;
	d4 = y - prev.y;

	if (ValidPixel(img,prev.x,prev.y)&&ValidPixel(img,next.x,prev.y))
	  Ix1 = d1*img->val[prev.x+img->tbrow[prev.y]] + 
	    d2*img->val[next.x+img->tbrow[prev.y]];
	else
	  Ix1 = img->val[u.x+img->tbrow[u.y]];

	if (ValidPixel(img,prev.x,next.y)&&ValidPixel(img,next.x,next.y))
	  Ix2 = d1*img->val[prev.x+img->tbrow[next.y]] + 
	    d2*img->val[next.x+img->tbrow[next.y]];
	else
	  Ix2 = img->val[u.x+img->tbrow[u.y]];
	
	If = d3*Ix1 + d4*Ix2;

	scl->val[v.x+scl->tbrow[v.y]] = (int)If;
      }
    }
  
  return(scl);
}

Image  *ImageMagnitude(Image *imgx, Image *imgy)
{
  Image *mag=CreateImage(imgx->ncols,imgx->nrows);
  int p,n=imgx->ncols*imgx->nrows;

  for (p=0; p < n; p++) 
    mag->val[p] = (int)sqrt(imgx->val[p]*imgx->val[p] + imgy->val[p]*imgy->val[p]);
  //    mag->val[p] = imgx->val[p]*imgx->val[p] + imgy->val[p]*imgy->val[p];

  return(mag);
}

Image *MakeImage(char *data)
{
  Image *img;
  int ncols,nrows,i,n;

  sscanf(data,"%d",&ncols);
  data=strchr(data,',')+1;
  sscanf(data,"%d",&nrows);
  data=strchr(data,',')+1;

  img = CreateImage(ncols,nrows);
  n   = ncols*nrows;
  for (i=0; i < n; i++) {
    sscanf(data,"%d",&img->val[i]);
    data=strchr(data,',')+1;
  }
  return(img);
}

Image  *AddZeros(Image *img, int ncols, int nrows)
{
  Image *eimg; /* expanded image with zeros to the left and to the
                  bottom of the input image */
  int i,j,x,y;

  eimg = CreateImage(img->ncols+ncols,img->nrows+nrows);

  j = 0;
  for (y=0; y < img->nrows; y++) 
    for (x=0; x < img->ncols; x++) {
      i = x + eimg->tbrow[y];      
      eimg->val[i]=img->val[j];
      j++;
    }
  return(eimg);
}

Image *CreateSine(int ncols, int nrows, float wx, float wy, float phix, float phiy) 
{
  int x, y, p;
  Image *simg = NULL;
  
  simg = CreateImage(ncols, nrows);
  
  /* We are adding a DC level of 127 to the amplitude of the
     sine. This will create an extra impulse at the center of the
     magnitude spectrum. */

  for (x = 0; x < ncols; x++) 
    for ( y = 0; y < nrows; y++) {
      p = x + simg->tbrow[y];
      simg->val[p] = (int) (127.0*(sin(2*PI*wx*(float)(x - phix)/ncols + 2*PI*wy*(float)(y - phiy)/nrows))+127.0+0.5);
    }
  
  return simg;
}

Image *CreateCos(int ncols, int nrows, float wx, float wy, float phix, float phiy)
{
  int x, y, p;
  Image *cosimg = NULL;
  
  cosimg = CreateImage(ncols, nrows);
  
  /* We are adding a DC level of 127 to the amplitude of the
     sine. This will create an extra impulse at the center of the
     magnitude spectrum. */
  
  for (x = 0; x < ncols; x++) 
    for ( y = 0; y < nrows; y++) {
      p = x + cosimg->tbrow[y];
      cosimg->val[p] = (int) (127.0*(cos(2*PI*wx*(float)(x - phix)/ncols + 2*PI*wy*(float)(y - phiy)/nrows))+127.0+0.5);
    }
  
  return cosimg;
}

Image  *CreateSquare(int xo, int yo, int xf, int yf, int ncols, int nrows, int val)
{
  Image *img=CreateImage(MAX(ncols,xf-xo+1),MAX(nrows,yf-yo+1));
  int x,y;

  for (y=yo; y < yf; y++) 
    for (x=xo; x < xf; x++) {
	img->val[x + img->tbrow[y]]=val;      
    }  
  return(img);
}

Image  *CreateIntArray(char *data)
{
  int ncols,nrows,i,n;
  Image *img;

  sscanf(data,"%d",&ncols);
  data=strchr(data,',')+1;
  sscanf(data,"%d",&nrows);
  data=strchr(data,',')+1;

  img = CreateImage(ncols, nrows);
  n   = ncols*nrows;
  for (i=0;i<n;i++) {
    sscanf(data,"%d",&img->val[i]);
    data=strchr(data,',')+1;
  }
  return(img);
}

Image *OverlayAlphaImage (Image *img, Image *alpha)
{
  Image *out=NULL;
  int i,n,imax;

  out = CreateImage(img->nrows,img->ncols);
  imax = MAX(MaximumValue(alpha),1);
  n = img->nrows * img->ncols;
  for (i=0; i<n; i++)
    out->val[i] = (int)img->val[i] * (float)alpha->val[i]/(float)imax;
  return (img);
}

int DistEucl(Image *img, int p, int q){
  int dx,dy;
  Pixel u,v;
  
  u.x = (p%img->ncols);
  u.y = (p/img->ncols);
  v.x = (q%img->ncols);
  v.y = (q/img->ncols);
  dx = v.x-u.x;
  dy = v.y-u.y;
  return(dx*dx + dy*dy); 
}
