#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "img_read.h"
#include "color_space.h"

namespace iread {

cv::Mat_<cv::Vec3d> IRead::img_read(const std::string &file_name, const std::string &colorspace, const int maxInt, const int verbosity)
{
	cv::Mat imageRaw = cv::imread(file_name, -1);
	cv::Mat_<cv::Vec3d> inputImage;
	if (imageRaw.type() == CV_16UC(3)) {
		cv::Mat_<cv::Vec3w> imageRawTyped = imageRaw;
		inputImage = cv::Mat_<cv::Vec3d>(imageRaw.rows, imageRaw.cols);

		double divisor = maxInt;
		if (divisor <= std::numeric_limits<double>::epsilon()) divisor = getMax(imageRawTyped);
		if (verbosity > 1) std::cout << "max = " << getMax(imageRawTyped) << std::endl;

		for (int y = 0; y < imageRaw.rows; ++y) {
			for (int x = 0; x < imageRaw.cols; ++x) {
				for (int c = 0; c < 3; ++c) {
					inputImage[y][x][c] = imageRaw.at<cv::Vec3w>(y, x)[c] / divisor;
					if (inputImage[y][x][c] > 1) inputImage[y][x][c] = 1;
				}
			}
		}
		if (verbosity > 0) std::cout << "16 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr((cv::Mat_<cv::Vec3d>)inputImage);
		} // rgb: do nothing

	} else { // 8 bit image
		if (verbosity > 0) std::cout << "8 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		} else {
			inputImage = ColorSpace::bgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		}
	}

	return inputImage;
}

cv::Mat_<cv::Vec3d> IRead::img_read_dl(
	const std::string &file_name,
	const std::string &colorspace,
	const int darklevel,
	const int maxInt,
	const int verbosity)
{

	cv::Mat imageRaw = cv::imread(file_name, -1);
	cv::Mat_<cv::Vec3d> inputImage;

	if (imageRaw.type() == CV_16UC(3)) {
		cv::Mat_<cv::Vec3w> imageRawTyped = imageRaw;
		inputImage = cv::Mat_<cv::Vec3d>(imageRaw.rows, imageRaw.cols);

		double divisor = maxInt;
		if (divisor <= std::numeric_limits<double>::epsilon()) divisor = getMax(imageRawTyped);
		if (verbosity > 1) std::cout << "max = " << getMax(imageRawTyped);

		for (int y = 0; y < imageRaw.rows; ++y) {
			for (int x = 0; x < imageRaw.cols; ++x) {
				cv::Vec3w &tmp = imageRaw.at<cv::Vec3w>(y, x);
				for (int c = 0; c < 3; ++c) {
					if (tmp[c] < darklevel) {
						inputImage[y][x][c] = 0;
					} else {
						inputImage[y][x][c] = (tmp[c] - darklevel) / divisor;
					}
				}
			}
		}

		if (verbosity > 0) std::cout << "16 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr((cv::Mat_<cv::Vec3d>)inputImage);
		} // rgb: do nothing

	} else { // 8 bit image

		if (verbosity > 0) std::cout << "8 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		cv::Mat_<cv::Vec3b> tmp;

		if (imageRaw.type() == CV_8UC(4)) {
			if (verbosity > 0) {
				std::cout << "WARNING: 4 channels 8 bit unsigned, I'll drop the alpha channel." << std::endl;
				std::cout << "WARNING: heuristics: assuming channel 3 to be the alpha channel. Double-check this if you get weird results." << std::endl;
			}

			// NOTE: experimental evaluation showed that PNG alpha channel is
			// number 3. No idea if that always holds.

			cv::Mat_<cv::Vec4b> other = imageRaw;
			tmp = cv::Mat_<cv::Vec3b>(other.rows, other.cols);
			for (int y = 0; y < other.rows; ++y) {
				for (int x = 0; x < other.cols; ++x) {
					for (int c = 0; c < 3; ++c) {
						tmp[y][x][c] = other[y][x][c];
					}
				}
			}
		} else {
			tmp = imageRaw;
		}

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr(tmp);
		} else {
			inputImage = ColorSpace::bgr2bgr(tmp);
		}
	}

	return inputImage;
}

// return also maximum intensity
cv::Mat_<cv::Vec3d> IRead::img_read_dl_max(
	const std::string &file_name,
	const std::string &colorspace,
	const int darklevel,
	int &maxInt,
	const int verbosity)
{

	cv::Mat imageRaw = cv::imread(file_name, -1);
	cv::Mat_<cv::Vec3d> inputImage;

	if (imageRaw.type() == CV_16UC(3)) {
		cv::Mat_<cv::Vec3w> imageRawTyped = imageRaw;
		inputImage = cv::Mat_<cv::Vec3d>(imageRaw.rows, imageRaw.cols);

		double divisor = maxInt;
		if (divisor <= std::numeric_limits<double>::epsilon()) { divisor = getMax(imageRawTyped); maxInt = (int)divisor; }
		if (verbosity > 1) std::cout << "max = " << getMax(imageRawTyped);

		for (int y = 0; y < imageRawTyped.rows; ++y) {
			for (int x = 0; x < imageRawTyped.cols; ++x) {
				cv::Vec3w &tmp = imageRawTyped[y][x];
				for (int c = 0; c < 3; ++c) {
					if (tmp[c] < darklevel) {
						inputImage[y][x][c] = 0;
					} else {
						inputImage[y][x][c] = (tmp[c] - darklevel) / divisor;
					}
				}
			}
		}

		if (verbosity > 0) std::cout << "16 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr((cv::Mat_<cv::Vec3d>)inputImage);
		} // rgb: do nothing

	} else { // 8 bit image

		if (verbosity > 0) std::cout << "8 bit" << std::endl;
		if (verbosity > 0) std::cout << "color space: " << colorspace << std::endl;

		if (colorspace.compare("srgb") == 0) {
			inputImage = ColorSpace::sbgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		} else {
			inputImage = ColorSpace::bgr2bgr((cv::Mat_<cv::Vec3b>)imageRaw);
		}
	}

	return inputImage;
}

int IRead::getMax(cv::Mat_<cv::Vec3b> &mat) {
	int max = -1;
	cv::Vec3d max3(-1, -1, -1);

	for (int y = 0; y < mat.rows; ++y)
		for (int x = 0; x < mat.cols; ++x)
			for (int c = 0; c < 3; ++c) {
				if (mat[y][x][c] > max)
					max = mat[y][x][c];
				if (mat[y][x][c] > max3[c]) max3[c] = mat[y][x][c];
			}

	std::cout << "max[0] = " << max3[0] << ", max[1] = " << max3[1] << ", max[2] = " << max3[2] << std::endl;
	return max;
}

int IRead::getMax(cv::Mat_<cv::Vec3w> &mat) {
	int max = -1;
	cv::Vec3d max3(-1, -1, -1);

	for (int y = 0; y < mat.rows; ++y)
		for (int x = 0; x < mat.cols; ++x)
			for (int c = 0; c < 3; ++c) {
				if (mat[y][x][c] > max)
					max = mat[y][x][c];
				if (mat[y][x][c] > max3[c]) max3[c] = mat[y][x][c];
			}

	std::cout << "max[0] = " << max3[0] << ", max[1] = " << max3[1] << ", max[2] = " << max3[2] << std::endl;
	return max;
}

int IRead::getMax(cv::Mat_<cv::Vec3d> &mat) {
	double max = -1;
	cv::Vec3d max3(-1, -1, -1);

	for (int y = 0; y < mat.rows; ++y)
		for (int x = 0; x < mat.cols; ++x)
			for (int c = 0; c < 3; ++c) {
				if (mat[y][x][c] > max)
					max = mat[y][x][c];
				if (mat[y][x][c] > max3[c]) max3[c] = mat[y][x][c];
			}

	std::cout << "max[0] = " << max3[0] << ", max[1] = " << max3[1] << ", max[2] = " << max3[2] << std::endl;
	return max;
}

}
