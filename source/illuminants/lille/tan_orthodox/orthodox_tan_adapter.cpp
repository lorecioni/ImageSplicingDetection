#include "orthodox_tan_adapter.h"
#include "zLightChroma.h"
#include <cassert>

namespace lille {

OrthodoxTanAdapter::OrthodoxTanAdapter()
{
}

illumestimators::Illum OrthodoxTanAdapter::estimate(cv::Mat_<cv::Vec3b> the_image)
{
	// translate image in Robby Tan's format
	 zArray2D<s_rgbi> img(the_image.rows, the_image.cols);
	  for (int y = 0; y < the_image.rows; ++y) {
		for (int x = 0; x < the_image.cols; ++x) {
			img[y][x].r = the_image[y][x][2];
			img[y][x].g = the_image[y][x][1];
			img[y][x].b = the_image[y][x][0];
		}
	  }


	zLightChroma zlc;

	// #2: to get an initial specular image = simg
	zArray2D<s_rgbi> simg;
	vector<s_rgbi> spec_init;
	zlc.zInitSpecular(img,simg,spec_init);

	// #3: to estimate illumination color from simg using the iic
	s_rgbi e;
	zlc.zEstimateLightColor(simg,spec_init, e);

	illumestimators::Illum ill;
	ill.r = e.r; ill.g = e.g; ill.b = e.b;
	ill.isValid = true;

	return ill;
}

illumestimators::Illum OrthodoxTanAdapter::estimate(cv::Mat_<cv::Vec3b> the_image, cv::Mat_<unsigned char> mask)
{
	// mask out white pixels
	assert((the_image.rows == mask.rows) && (the_image.cols == mask.cols));

	// alter copy, not original image;
	cv::Mat_<cv::Vec3d> use_this;
	the_image.copyTo(use_this);

	for (int y = 0; y < the_image.rows; ++y)
		for (int x = 0; x < the_image.cols; ++x)
			if (mask[y][x] == 255)
				use_this[y][x] = cv::Vec3d(0, 0, 0);

	return estimate(use_this);
}



}
