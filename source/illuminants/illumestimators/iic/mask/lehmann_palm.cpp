#include "lehmann_palm.h"

namespace iic {

LehmannPalm::LehmannPalm(cv::Mat_<cv::Vec3b> image, double s, double i) {
	mask = cv::Mat_<uchar>(image.rows, image.cols, (uchar)0);
	// find i_max
	double i_max = 0; double s_max = 0;
	double thisI, thisS;
	for (int y = 0; y < image.rows; ++y) { // compute i_max
		for (int x = 0; x < image.cols; ++x) {
			thisI = (image[y][x][0]+image[y][x][1]+image[y][x][2])/3.0;
			thisS = 1.0-(std::min<uchar>(image[y][x][0], std::min<uchar>(image[y][x][1], image[y][x][2]))
				/ std::max<uchar>(1.0/3.0, thisI));
			if (thisI > i_max) i_max = thisI;
			if (thisS > s_max) s_max = thisS;
		}
	}
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; ++x) {
			thisI = (image[y][x][0]+image[y][x][1]+image[y][x][2])/3.0;
			thisS = 1.0-(std::min<uchar>(image[y][x][0], std::min<uchar>(image[y][x][1], image[y][x][2]))
				/ std::max<uchar>(1.0/3.0, thisI));
			
			if ((thisI > i*i_max) && (thisS < s*s_max))
				mask[y][x] = 255;
		}
	}
}

// code repetition
LehmannPalm::LehmannPalm(cv::Mat_<cv::Vec3s> image, double s, double i) {
	mask = cv::Mat_<uchar>(image.rows, image.cols, (uchar)0);
	// find i_max
	double i_max = 0; double s_max = 0;
	double thisI, thisS;
	for (int y = 0; y < image.rows; ++y) { // compute i_max
		for (int x = 0; x < image.cols; ++x) {
			thisI = (image[y][x][0]+image[y][x][1]+image[y][x][2])/3.0;
			thisS = 1.0-(std::min<uchar>(image[y][x][0], std::min<uchar>(image[y][x][1], image[y][x][2]))
				/ std::max<uchar>(1.0/3.0, thisI));
			if (thisI > i_max) i_max = thisI;
			if (thisS > s_max) s_max = thisS;
		}
	}
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; ++x) {
			thisI = (image[y][x][0]+image[y][x][1]+image[y][x][2])/3.0;
			thisS = 1.0-(std::min<uchar>(image[y][x][0], std::min<uchar>(image[y][x][1], image[y][x][2]))
				/ std::max<uchar>(1.0/3.0, thisI));
			
			if ((thisI > i*i_max) && (thisS < s*s_max))
				mask[y][x] = 255;
		}
	}
}

cv::Mat_<uchar> LehmannPalm::getMask() const
{
	assert((mask.rows > 0) && (mask.cols > 0)); // already computes a non-trivial mask?
	return mask;
}


}
