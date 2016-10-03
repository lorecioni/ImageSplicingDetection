// author: robby tan
// zLightChroma.cxx
//////////////////////////////////////////////////////////////////////

#include "zLightChroma.h"

#define CAMERA_DARK 5


zLightChroma::zLightChroma()
{

/*
  // #1: to load an image
  zArray2D<s_rgbi> img;
  ::zReadImage("img/ball-blue-green-2800.ppm",img);
  //::zShow(img);

  // #2: to get an initial specular image = simg
  zArray2D<s_rgbi> simg;
  vector<s_rgbi> spec_init;
  zInitSpecular(img,simg,spec_init);

  // #3: to estimate illumination color from simg using the iic
  s_rgbi e;
  zEstimateLightColor(simg,spec_init, e);
  printf("light color : %f, %f, %f\n",e.r,e.g,e.b);

  // #4: to show the results
  //zShowColor(e);  
  zNormalization(img, e);  
  ::zWriteImage(img, "result.ppm");
  printf("the normalization result was written in 'result.ppm'.\n");
*/
}

zLightChroma::~zLightChroma()
{

}


void zLightChroma::zInitSpecular(zArray2D<s_rgbi> &img, zArray2D<s_rgbi> &simg,
				vector<s_rgbi> &spec_init)
{
  // *************************************************
  // to find initial specular based-on Intensity
  // *************************************************

  simg.zAllocate(img.zGetMaxY(),img.zGetMaxX());

  float maxI = 0;
  float minI = 1000;
  for(int y=0;y<img.zGetMaxY();y++){
    for(int x=0;x<img.zGetMaxX();x++){
      s_rgbi iro = img(y,x);
      if(iro.r <= CAMERA_DARK ||  iro.g <= CAMERA_DARK || iro.b <= CAMERA_DARK)
	continue;

      if(maxI<iro.zTotal()) maxI=iro.zTotal();
      if(minI>iro.zTotal()) minI=iro.zTotal();

    }
  }
	
  float th=50; //threshold
  float specTh= maxI - ((maxI-minI)*th/100.f); 
	
  for(int y=0;y< img.zGetMaxY();y++){
    for(int x=0;x<img.zGetMaxX();x++){

      s_rgbi iro = img(y,x);

      // to ignore the dark
      if(iro.r <= CAMERA_DARK ||  iro.g <= CAMERA_DARK || iro.b <= CAMERA_DARK)
	continue;

      // to ignore the bright
      if(iro.r>= 250 || iro.g>=250 || iro.b>250)
	continue;

      if(iro.zTotal() > specTh)	simg(y,x)=iro;

    }
  }
  //::zShow(simg,"initial regions of specularity");
  
  //zHighlightRegion_Lehmann(_img,_spec);
  
  
  // to have the vector of the specular image: initSpec
  zArray3D<bool> map;
  map.zAllocate(256,256,256);
  
  for(int y=0;y<simg.zGetMaxY();y++){
    for(int x=0;x<simg.zGetMaxX();x++){
      s_rgbi iro = simg(y,x);
      
      if(iro.r<CAMERA_DARK || iro.g<CAMERA_DARK || iro.b <CAMERA_DARK)
	continue;
      
      if(map((int)iro.r,(int)iro.g,(int)iro.b)) continue;

      spec_init.push_back(iro);
      map((int)iro.r,(int)iro.g,(int)iro.b)=true;
    }
  }
  
}


void zLightChroma::zEstimateLightColor(zArray2D<s_rgbi> &simg,
				      vector<s_rgbi> &spec_init, s_rgbi &e)
{
  // *************************************************
  // to estimate light chromatcity based on IIC
  // *************************************************
  
  vector<s_xy<float> > ra,ga,ba;
  
  zArray2D<char> rmap,gmap,bmap;
  rmap.zAllocate(1000,1000);
  gmap.zAllocate(1000,1000);
  bmap.zAllocate(1000,1000);


  for(int i=0;i<spec_init.size();i++){

    s_rgbi iro  = spec_init.at(i);
    
    if(iro.r<CAMERA_DARK || iro.g<CAMERA_DARK || iro.b <CAMERA_DARK)
      continue;


    float r=iro.zChroma_r();
    float g=iro.zChroma_g();
    float b=iro.zChroma_b();
    
    s_xy<float> d;
    
    //--------------------------------------
    d.y=r;
    d.x=(1/iro.zTotal())*10000;

    int dum1,dum2;
    
    dum1=(int)floor(fabs(d.y*1000));
    dum2=(int)floor(fabs(d.x));
			
    if(dum1>1000 || dum2 >1000) 
      continue;
    if(dum1<0 || dum2<0) 
      continue;
    
    if(!rmap(dum1,dum2)){
      ra.push_back(d);
      rmap(dum1,dum2)=1;
    }
    
    //--------------------------------------
    d.y=g;
    d.x=(1/iro.zTotal())*10000;
    
    dum1=(int)floor(fabs(d.y*1000));
    dum2=(int)floor(fabs(d.x));
    
    if(dum1>1000 || dum2 >100) 
      continue;
    if(dum1<0 || dum2<0) 
      continue;
    
    if(!gmap(dum1,dum2)){
      ga.push_back(d);
      gmap(dum1,dum2)=1;
    }
    
    //--------------------------------------
    d.y=b;
    d.x=(1/iro.zTotal())*10000;
    
    
    dum1=(int)floor(fabs(d.y*1000));
    dum2=(int)floor(fabs(d.x));
    
    if(dum1>1000 || dum2 >1000) 
      continue;
    if(dum1<0 || dum2<0) 
      continue;
    
    if(!bmap(dum1,dum2)){
      ba.push_back(d);
      bmap(dum1,dum2)=1;
    }
    //--------------------------------------
    
    
  }

  zHoughSpace(ra,e.r,"har.csv");
  zHoughSpace(ga,e.g,"hag.csv");
  zHoughSpace(ba,e.b,"hab.csv");

}


void zLightChroma::zHoughSpace(vector<s_xy<float> > &iic, float &c, const char *debugFile)
{
  // *************************************************
  // to compute the intersection in hough space
  // *************************************************

  // y = m*x + b

  int start_b=100;
  int end_b = 990;

  int maxm = 3000;
  int maxb = end_b;

  zArray2D<int> param_space; // y = m ; x = b
  param_space.zAllocate(maxm,maxb);

  vector<s_xy<int> > param_idx;
  
  for(int i=0;i<iic.size();i++){
    float y=iic.at(i).y;
    float x=iic.at(i).x/10000;

    for(int b=start_b;b<end_b;b++){

      float m = (((float)b/1000)/x)-(y/x);
      m=floor(2*m+0.5); // THIS MULTIPLICATION must be inteligently set

      // this can make some mistakes, but it would be fine in most cases
      int im = (int)fabs(m); 

      if(im>=maxm) continue;

      if(param_space(im,b)==10){
	s_xy<int> dum;
	dum.x = b;
	dum.y = im;
	
	param_idx.push_back(dum);
      }

      param_space(im,b)++;
		
    }
  }


  /****** using average value of N% of curve tip *****/
  
  vector<int> b_histo(1000);
  
  for(int i=0;i<param_idx.size();i++){
    if(param_space(param_idx.at(i).y,param_idx.at(i).x)<10) 
      continue;

    b_histo.at(param_idx.at(i).x)
      +=param_space(param_idx.at(i).y,param_idx.at(i).x);
    
  }
  
  /* *******************	
  for(int b=start_b;param<end_b;b++){  //bingDebug
    ::zWrite2csv((float)b/1000.0f,b_histo.at(b),0,0,debugFile);
  }
  / ******************/
  
  int max=0, candid;
  
  vector<int> bestN;
  int N=10;
  
  for(int i=0;i<N;i++){
    for(int b=start_b;b<end_b;b++){
      if(max<b_histo.at(b)){
	max=b_histo.at(b);
	candid=b;
      }
    }
    if(candid>0 && candid<1000){
      bestN.push_back(candid);
      b_histo.at(candid) = 0;
    }
    max=0;
    candid=0;
  }
  
  float chroma=0;
  for(int i=0;i<N;i++){
    if(i<bestN.size())
      chroma+=bestN.at(i);
  }
  chroma/=(float)N;
  
  c=chroma/1000.f;
}


void zLightChroma::zShowColor(s_rgbi &iro)
{
  zArray2D<s_rgbi> img(100,100);
  for(int y=0;y<img.zGetMaxY();y++){
    for(int x=0;x<img.zGetMaxX();x++){
      img(y,x).r=iro.r*300;
      img(y,x).g=iro.g*300;
      img(y,x).b=iro.b*300;
    }
  }
  //::zShow(img);
}

void zLightChroma::zNormalization(zArray2D<s_rgbi> &img,s_rgbi &e)
{
  float max,med,min;
  ::zSortRGB(e.r,e.g,e.b,max,med,min);
  e.r/=med;
  e.g/=med;
  e.b/=med;

  zArray2D<s_rgbi> nimg;
  nimg.zCopyIn(img);

  for(int y=0;y<img.zGetMaxY();y++){
    for(int x=0;x<img.zGetMaxX();x++){

      float r = (int) floor(img(y,x).r/e.r);
      float g = (int) floor(img(y,x).g/e.g);
      float b = (int) floor(img(y,x).b/e.b);
      if(r>254) r=255;
      if(g>254) g=255;
      if(b>254) b=255;
      if(r<0) r=0;
      if(g<0) g=0;
      if(b<0) b=0;
      nimg(y,x).r=r*0.7f;
      nimg(y,x).g=g*0.7f;
      nimg(y,x).b=b*0.7f;

    }
  }

  //::zShow(nimg);
  img.zCopyIn(nimg);
}


void zLightChroma::zHighlightRegion_Lehmann(zArray2D<s_rgbi> &in,
					    zArray2D<s_rgbi> &out)
{
  float Imax=0, Smax=0,R,G,B;

  for(int y=0;y<in.zGetMaxY();y++){
    for(int x=0;x<in.zGetMaxX();x++){
      R=in(y,x).r;
      G=in(y,x).g;
      B=in(y,x).b;
      if(R>253 ||  G>253 || B>253) continue;
      if(R<CAMERA_DARK || G< CAMERA_DARK || B<CAMERA_DARK) continue;
	 
      float I=(R+G+B)/3.0f;
      float S=1-(::zGetMin(R,G,B)/I);
      if(Imax<I) Imax=I;
      if(Smax<S) Smax=S;

    }
  }
  
  out.zAllocate(in.zGetMaxY(),in.zGetMaxX());
  for(int y=0;y<in.zGetMaxY();y++){
    for(int x=0;x<in.zGetMaxX();x++){
      R=in(y,x).r;
      G=in(y,x).g;
      B=in(y,x).b;
      if(R>253 ||  G>253 || B>253) continue;
      if(R<CAMERA_DARK || G< CAMERA_DARK || B<CAMERA_DARK) continue;

      float I=(R+G+B)/3.0f;
      float S=1-(::zGetMin(R,G,B)/I);
      if((I>Imax*0.5) && (S<Smax*0.8)) //threshold
	 out(y,x)=in(y,x);

    }
  }
  
  //::zShow(out);

}

