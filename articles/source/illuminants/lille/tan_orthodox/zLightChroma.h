// author: robby tan
// zLightChroma.h
//////////////////////////////////////////////////////////////////////

#ifndef _zLightChroma_H__
#define _zLightChroma_H__

#include "zGlobal.h"

class zLightChroma  
{
 protected:

 public:
  void zInitSpecular(zArray2D<s_rgbi> &img, zArray2D<s_rgbi> &simg,
		    vector<s_rgbi> &spec_init);
  void zEstimateLightColor(zArray2D<s_rgbi> &simg, vector<s_rgbi> &spec_init,
			  s_rgbi &e);
  void zHoughSpace(vector<s_xy<float> > &iic, float &c, const char *dfile);

  void zShowColor(s_rgbi &e);
  void zNormalization(zArray2D<s_rgbi> &img, s_rgbi &e);

  void zHighlightRegion_Lehmann(zArray2D<s_rgbi> &in,zArray2D<s_rgbi> &out);

  
 public:
  zLightChroma();
  virtual ~zLightChroma();

};

#endif 
