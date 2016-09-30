#include "illuminantestimator.h"
#include "common/color.h"
#include "common/mask.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cassert>

namespace illumestimators {

IlluminantEstimator::IlluminantEstimator()
{

}

IlluminantEstimator::~IlluminantEstimator()
{

}

Illum IlluminantEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image) const
{
	cv::Mat_<unsigned char> mask = cv::Mat_<unsigned char>::zeros(image.rows, image.cols);

	return estimateIlluminant(image, mask);
}

Illum IlluminantEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel) const
{
	cv::Mat_<unsigned char> mask = cv::Mat_<unsigned char>::zeros(image.rows, image.cols);

	return estimateIlluminant(image, superpixel, mask);
}

Illum IlluminantEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	cv::Mat_<unsigned char> superpixelMask = Mask::fromSuperpixel(image, superpixel);

	assert((mask.rows == superpixelMask.rows) && (mask.cols == superpixelMask.cols));
	cv::Mat_<unsigned char> completeMask(mask.rows, mask.cols, (unsigned char)0);

//	NUR AUF cvArr * definiert!
//	cvAnd(&mask,&superpixelMask, &completeMask);
//	manuell implementiert:
	for (int y = 0; y < mask.rows; ++y) {
		for (int x = 0; x < mask.cols; ++x) {
			unsigned char tmp = std::max<unsigned char>(mask[y][x], superpixelMask[y][x]);
			if (tmp > 0)
				completeMask[y][x] = tmp;
		}
	}
	return estimateIlluminant(image, completeMask);
}

void IlluminantEstimator::estimateIlluminants(const cv::Mat_<cv::Vec3d>& image, const std::vector<superpixels::Superpixel>& superpixels, std::vector<Illum>& estimatedIlluminants, const cv::Mat_<unsigned char>& mask) const
{
	estimatedIlluminants.clear();

	for (unsigned int i = 0; i < superpixels.size(); i++) {
		Illum estimate = estimateIlluminant(image, superpixels[i], mask);

		estimatedIlluminants.push_back(estimate);
	}
}

// returns an image that is normed between 0 and 1 (where "1" is 3690, fixed value!), no matter whether original input format is 8bits or 16bits
cv::Mat_<cv::Vec3d> IlluminantEstimator::cleanlyReadImage(const std::string &imageFile, const std::string &colorspace, int verbosity) {

	cv::Mat imageRaw = cv::imread(imageFile, -1);
	cv::Mat_<cv::Vec3d> inputImage;
	if (imageRaw.type() == CV_16UC(3)) {
		if (verbosity > 0) std::cout << "16 bit" << std::endl;
		inputImage = cv::Mat_<cv::Vec3d>(imageRaw.rows, imageRaw.cols);
		// chop down, as in gamut mapping
		// optionally, subtract black level
		if (imageFile.find("IMG_") != std::string::npos) { // subtract black level
			if (verbosity > 0) std::cout << "- darklevel" << std::endl;
			for (int y = 0; y < imageRaw.rows; ++y) {
				for (int x = 0; x < imageRaw.cols; ++x) {
					cv::Vec3w &tmp = imageRaw.at<cv::Vec3w>(y, x);
					for (int c = 0; c < 3; ++c) {
						if (tmp[c] < 129) {
							inputImage[y][x][c] = 0;
						} else {
							inputImage[y][x][c] = (tmp[c] - 129.0) / 3690.0;
						}
					}
				}
			}
		} else {
			if (verbosity > 0) std::cout << "dl = 0" << std::endl;
			for (int y = 0; y < imageRaw.rows; ++y) {
				for (int x = 0; x < imageRaw.cols; ++x) {
					for (int c = 0; c < 3; ++c) {
						inputImage[y][x][c] = imageRaw.at<cv::Vec3w>(y, x)[c] / 3690.0;
						if (inputImage[y][x][c] > 1) inputImage[y][x][c] = 1;
					}
				}
			}
		}
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;
		if (colorspace.compare("srgb") == 0) {
			inputImage = Color::sbgr2bgr((cv::Mat_<cv::Vec3d>)inputImage);
		} // rgb: do nothing
	} else { // 8 bit image
		if (verbosity > 0) std::cout << "8 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;
		if (colorspace.compare("srgb") == 0) {
			inputImage = Color::sbgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		} else {
			inputImage = Color::bgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		}
	}

	return inputImage;
}

} // namespace illumestimators
