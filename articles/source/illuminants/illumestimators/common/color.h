#ifndef ILLUMESTIMATORS_COMMON_COLOR_H
#define ILLUMESTIMATORS_COMMON_COLOR_H

#include <opencv2/core/core.hpp>
#include "illum.h"

namespace illumestimators {

/** The Color class provides helper functions for colorspace conversions */
class Color
{
public:
	static cv::Vec3d calculateChromaticities(cv::Vec3d color);
	static Illum calculateChromaticities(Illum color);
	static cv::Mat_<cv::Vec3d> colorCorrectImage(const cv::Mat_<cv::Vec3d>& image, const cv::Vec3d& estimate);

public:
	static cv::Mat_<cv::Vec3d> srgb2bgr(const unsigned char* input, int rows, int cols);
	static cv::Mat_<cv::Vec3d> rgb2bgr(const unsigned char* input, int rows, int cols);

	static cv::Mat_<cv::Vec3d> sbgr2bgr(const cv::Mat_<cv::Vec3b>& input);
	static cv::Mat_<cv::Vec3d> bgr2bgr(const cv::Mat_<cv::Vec3b>& input);

	static cv::Vec3d rgb2bgr(const cv::Vec3d& input);
	static cv::Vec3d bgr2rgb(const cv::Vec3d& input);

	static cv::Mat_<cv::Vec3d> sbgr2bgr(const cv::Mat_<cv::Vec3d>& input);
	static cv::Mat_<cv::Vec3d> bgr2sbgr(const cv::Mat_<cv::Vec3d>& input);
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMON_COLOR_H
