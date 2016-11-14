// zImageMagick.h is an interface to ImageMagick++


#ifndef __zIMAGEMAGICK_H__
#define __zIMAGEMAGICK_H__

#include "zArray.h"
#include "zGlobal.h"
#include "Magick++.h"

using namespace Magick;

namespace privateMagick{
  inline int zMagick2Array(Image &img, zArray2D<s_rgbi> &arr)
    {
      // to convert  to Image class zArray2D<s_rgbi>

      if(img.channelDepth(RedChannel)>8){
	img.channelDepth(RedChannel, 8);
	img.channelDepth(GreenChannel, 8);
	img.channelDepth(BlueChannel, 8);
      }

      int maxY = img.rows();
      int maxX = img.columns();
      PixelPacket *pix =img.getPixels(0,0,maxX,maxY);
      arr.zAllocate(maxY,maxX);

      for(int y=0;y<maxY;y++){
	for(int x=0;x<maxX;x++){
	  arr(y,x).r = (unsigned char) (pix+y*maxX+x)->red;
	  arr(y,x).g = (unsigned char) (pix+y*maxX+x)->green;
	  arr(y,x).b = (unsigned char) (pix+y*maxX+x)->blue;
	}
      }
      return 0;
    }
  
  inline int zMagick2Array(Image &img, zArray2D<float> &arr)
    {
      // to convert  to Image class zArray2D<float>
      if(img.channelDepth(RedChannel)>8){
	img.channelDepth(RedChannel, 8);
	img.channelDepth(GreenChannel, 8);
	img.channelDepth(BlueChannel, 8);
      }
      
      int maxY = img.rows();
      int maxX = img.columns();
      PixelPacket *pix =img.getPixels(0,0,maxX,maxY);
      arr.zAllocate(maxY,maxX);
      for(int y=0;y<maxY;y++){
	for(int x=0;x<maxX;x++){
	  arr(y,x) = (unsigned char) (pix+y*maxX+x)->red;
	    /*********
	    (unsigned char)::zColor2Gray( (pix+y*maxX+x)->red,
						   (pix+y*maxX+x)->green,
						   (pix+y*maxX+x)->blue);
	    /*********/
	}
      }
      return 0;
    }

  inline int zArray2Magick(const zArray2D<s_rgbi> &arr,Image &img)
    {
      // to convert zArray2D<s_rgbi> to Image class 

      int maxY = arr.zGetMaxY();
      int maxX = arr.zGetMaxX();
      
      unsigned char *pix = new unsigned char[maxX*maxY*3];
      
      int i=0;
      for(int y=0;y<maxY;y++){
	for(int x=0;x<maxX;x++){
	  pix[i++]=(unsigned char) floor(arr(y,x).r);
	  pix[i++]=(unsigned char) floor(arr(y,x).g);
	  pix[i++]=(unsigned char) floor(arr(y,x).b);
	}
      }
      img.read(maxX,maxY,"RGB",CharPixel,pix);
      img.channelDepth(RedChannel, 8);
      img.channelDepth(GreenChannel, 8);
      img.channelDepth(BlueChannel, 8);

      delete[] pix;
      return 0;
    } 
  

  
  inline int zArray2Magick(const zArray2D<float> &arr,Image &img)
    {
      // to convert zArray2D<float> to Image class 
      
      int maxY = arr.zGetMaxY();
      int maxX = arr.zGetMaxX();
      
      unsigned char *pix = new unsigned char[maxX*maxY*3];
      
      int i=0;
      for(int y=0;y<maxY;y++){
	for(int x=0;x<maxX;x++){
	  pix[i++]=(unsigned char) floor(arr(y,x));
	  pix[i++]=(unsigned char) floor(arr(y,x));
	  pix[i++]=(unsigned char) floor(arr(y,x));
	}
      }
      img.read(maxX,maxY,"RGB",CharPixel,pix);
      img.channelDepth(RedChannel, 8);
      img.channelDepth(GreenChannel, 8);
      img.channelDepth(BlueChannel, 8);

      delete[] pix;
      return 0;
    }
  
}

namespace zImageMagick{

  inline int zReadImage(const char *fname, zArray2D<s_rgbi> &img)
    {
      FILE *fh;
      if((fh=fopen(fname,"r"))){
	fclose(fh);
	Image imgm(fname); 
	if(imgm.columns()*imgm.rows()>0){
	  privateMagick::zMagick2Array(imgm,img);
	  return 0;
	}
      }
      printf("Image NOT  Found\n");
      exit(-1);
      return -1;
    }
  
  inline int zReadImageGray(const char *fname, zArray2D<float> &img)
    {
      zArray2D<s_rgbi> iro;
      zReadImage(fname,iro);
      
      img.zAllocate(iro.zGetMaxY(),iro.zGetMaxX());
      for(int y=0;y<iro.zGetMaxY();y++){
	for(int x=0;x<iro.zGetMaxX();x++){
	  img(y,x)=iro(y,x).r;
	}
      }
      return 0;
    }
  
  inline int zWriteImage(const zArray2D<s_rgbi> &arr, const char *fname)
    {
      Image img;
      privateMagick::zArray2Magick(arr,img);
      img.write(fname);
      
      return 0;
    }
  
  inline int zWriteImageGray(const zArray2D<float> &arr, const char *fname)
    {
      zArray2D<s_rgbi> dum(arr.zGetMaxY(),arr.zGetMaxX());
      
      for(int y=0;y<dum.zGetMaxY();y++){
	for(int x=0;x<dum.zGetMaxX();x++){
	  dum(y,x).r= dum(y,x).g= dum(y,x).b= arr(y,x);
	}
      }
      
      zWriteImage(dum,fname);
      return 0;
    }

  inline int zShow(const zArray2D<s_rgbi> &arr, const char *msg=0)
    {
      // to display an image that has s_rgbi type of data
      
      //::zWriteImage(img,"a.bmp");
      //spawnl(_P_NOWAIT,"imageviewer","imageviewer","a.bmp",NULL);
      if(msg!=0) printf("\nclose image to continue...[%s]\n", msg);
      else printf("close image to continue...\n");
      Image imgm;
      privateMagick::zArray2Magick(arr, imgm);
      imgm.display();
      return 0;
    }
  
  
  inline int zShow(const zArray2D<float> &img, const char *msg=0)
    {
      // to display an image that has float type of data
      
      zArray2D<s_rgbi> arr;
      arr.zAllocate(img.zGetMaxY(),img.zGetMaxX());
      
      for(int y=0;y<arr.zGetMaxY();y++){
	for(int x=0;x<arr.zGetMaxX();x++){
	  float s=arr(y,x).b=arr(y,x).g=arr(y,x).r=floor(img(y,x));
	}
      }
     zShow(arr, msg);
     return 0;
    }

  inline int zShow(const zArray2D<double> &img, const char *msg=0)
    {
      // to display an image that has float type of data
      
      zArray2D<s_rgbi> arr;
      arr.zAllocate(img.zGetMaxY(),img.zGetMaxX());
      
      for(int y=0;y<arr.zGetMaxY();y++){
	for(int x=0;x<arr.zGetMaxX();x++){
	  float s=arr(y,x).b=arr(y,x).g=arr(y,x).r= (float) floor(img(y,x));
	}
      }
     zShow(arr, msg);
     return 0;
    }
  
  
  inline int zBlur(zArray2D<float> &arr,const double width=5, 
		   const double sigma=5)
    {
      // to blur an image using magick++
      
      Image img;
      privateMagick::zArray2Magick(arr, img);
      img.gaussianBlur(width,sigma);
      //img.display();
      arr.zDestroy();
      privateMagick::zMagick2Array(img,arr);
      return 0;
    }

  inline int zBlur(zArray2D<s_rgbi> &arr,const double width=5, 
		   const double sigma=5)
    {
      // to blur an image using magick++
      
      Image img;
      privateMagick::zArray2Magick(arr, img);
      img.gaussianBlur(width,sigma);
      privateMagick::zMagick2Array(img,arr);
      return 0;
    }


  inline int zEdge(const zArray2D<s_rgbi> &arr,zArray2D<float> &edge, const float th=0.0f)
    {
      // to detect edges using magick++
      
      Image img;
      privateMagick::zArray2Magick(arr, img);
      img.edge(th);
      privateMagick::zMagick2Array(img,edge);
      return 0;
    }
  



}


#endif // __zIMAGEMAGICK_H__
