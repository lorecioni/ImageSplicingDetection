#include "mask.h"
#include <math.h>
#include <algorithm>

namespace illumestimators {

std::vector<cv::Vec3d> Mask::maskedPixels(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask)
{
	std::vector<cv::Vec3d> pixels;

	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return pixels;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (mask(y, x) > 0) {
				pixels.push_back(image(y, x));
			}
		}
	}

	return pixels;
}

std::vector<cv::Vec3d> Mask::unmaskedPixels(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask)
{
	std::vector<cv::Vec3d> pixels;

	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return pixels;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (mask(y, x) == 0) {
				pixels.push_back(image(y, x));
			}
		}
	}

	return pixels;
}

std::vector<cv::Vec3d> Mask::maskedPixels(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel, const cv::Mat_<unsigned char>& mask)
{
	const std::vector<cv::Point> &coords = superpixel.coordinates;
	std::vector<cv::Vec3d> pixels;

	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return pixels;
	}

	for(std::vector<cv::Point>::const_iterator it = coords.begin(); it != coords.end(); it++) {
		cv::Point coord = *it;

		if (mask(coord.y, coord.x) > 0) {
			pixels.push_back(image(coord.y, coord.x));
		}
	}

	return pixels;
}

std::vector<cv::Vec3d> Mask::unmaskedPixels(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel, const cv::Mat_<unsigned char>& mask)
{
	const std::vector<cv::Point> &coords = superpixel.coordinates;
	std::vector<cv::Vec3d> pixels;

	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return pixels;
	}


	for(std::vector<cv::Point>::const_iterator it = coords.begin(); it != coords.end(); it++) {
		if (mask(it->y, it->x) == 0) {
			pixels.push_back(image(it->y, it->x));
		}
	}

	return pixels;
}

void Mask::maskSaturatedPixels(const cv::Mat_<cv::Vec3d>& image, cv::Mat_<unsigned char>& mask, double threshold)
{
	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (std::max(image(y, x)[0], std::max(image(y, x)[1], image(y, x)[2])) > threshold) {
				mask(y, x) = 255;
			}
		}
	}
}

void Mask::maskMinPixels(const cv::Mat_<cv::Vec3d>& image, cv::Mat_<unsigned char>& mask, double threshold)
{
	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if (std::min(image(y, x)[0], std::min(image(y, x)[1], image(y, x)[2])) < threshold) {
				mask(y, x) = 255;
			}
		}
	}
}

void Mask::maskDarkPixels(const cv::Mat_<cv::Vec3d>& image, cv::Mat_<unsigned char>& mask, double threshold)
{
	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if ((image(y, x)[0] + image(y, x)[1] + image(y, x)[2]) < threshold) {
				mask(y, x) = 255;
			}
		}
	}
}

void Mask::maskBorderPixels(const cv::Mat_<cv::Vec3d>& image, cv::Mat_<unsigned char>& mask, int width)
{
	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
		std::cerr << "Image and mask size mismatch!" << std::endl;

		return;
	}

	if (width < 1) {
		return;
	}

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			if ((y < width) || (y >= image.rows - width) || (x < width) || (x >= image.cols - width)) {
				mask(y, x) = 255;
			}
		}
	}
}

cv::Mat_<unsigned char> Mask::fromSuperpixel(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel, bool maskSuperpixel)
{
	cv::Mat_<unsigned char> mask;// (image.rows, image.cols);

//	if ((mask.rows != image.rows) || (mask.cols != image.cols)) {
//		std::cerr << "Image and mask size mismatch!" << std::endl;
//
//		return mask;
//	}

	if (maskSuperpixel) {
		mask = cv::Mat_<unsigned char>(image.rows, image.cols, (unsigned char)0);
	} else {
		mask = cv::Mat_<unsigned char>(image.rows, image.cols, (unsigned char)255);
	}

	if (maskSuperpixel) {
		for (unsigned int i = 0; i < superpixel.coordinates.size(); i++) {
			mask[superpixel.coordinates[i].y][superpixel.coordinates[i].x] = 255;
		}
	} else {
		for (unsigned int i = 0; i < superpixel.coordinates.size(); i++) {
			mask[superpixel.coordinates[i].y][superpixel.coordinates[i].x] = 0;
		}
	}

	return mask;
}

} // namespace illumestimators
