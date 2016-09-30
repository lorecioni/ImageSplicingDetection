#include "derivative.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

namespace illumestimators {

double Derivative::gaussianDerivativeFunction(double x, int order, double sigma)
{
	const double sigma2 = sigma * sigma;
	const double x2 = x * x;

	const double gaussian = 1 / (sqrt(2 * M_PI) * sigma) * exp(x2 / (-2 * sigma2));

	if (order == 1) {
		return (-x / sigma2) * gaussian;
	} else if (order == 2) {
		return (1 / sigma2) * ((x2 / sigma2) - 1) * gaussian;
	}

	return gaussian;
}

void Derivative::createGaussianDerivativeKernel(cv::Mat_<double>& filter, int kernelsize, int order, double sigma)
{
	const int filtersize = ((kernelsize - 1) / 2);

	filter = cv::Mat_<double>(1, kernelsize);

	for (int x = -filtersize; x <= filtersize; x++) {
		filter(0, x + filtersize) = gaussianDerivativeFunction(x, order, sigma);
	}

	if (order == 0) {
		double sum = 0;
		for (int x = -filtersize; x <= filtersize; x++) {
			sum += filter(0, x + filtersize);
		}
		for (int x = -filtersize; x <= filtersize; x++) {
			filter(0, x + filtersize) /= sum;
		}
	} else if (order == 1) {
		double sum = 0;
		for (int x = -filtersize; x <= filtersize; x++) {
			sum += x * filter(0, x + filtersize);
		}
		for (int x = -filtersize; x <= filtersize; x++) {
			filter(0, x + filtersize) /= sum;
		}
	} else if (order == 2) {
		double sum = 0;
		for (int x = -filtersize; x <= filtersize; x++) {
			sum += filter(0, x + filtersize);
		}
		for (int x = -filtersize; x <= filtersize; x++) {
			filter(0, x + filtersize) -= sum / kernelsize;
		}

		sum = 0;
		for (int x = -filtersize; x <= filtersize; x++) {
			sum += 0.5 * x * x * filter(0, x + filtersize);
		}
		for (int x = -filtersize; x <= filtersize; x++) {
			filter(0, x + filtersize) /= sum;
		}
	}
}

cv::Mat_<cv::Vec3d> Derivative::gaussianDerivativeFilter(const cv::Mat_<cv::Vec3d>& input, int xorder, int yorder, double sigma)
{
	if (sigma <= 0) {
		return cv::Mat_<cv::Vec3d>::zeros(input.rows, input.cols);
	}

	cv::Mat_<cv::Vec3d> output(input.rows, input.cols);

	const int kernelsize = cvRound(sigma * 3 * 2 + 1) | 1;

	cv::Mat_<double> filterX;
	createGaussianDerivativeKernel(filterX, kernelsize, xorder, sigma);

	cv::Mat_<double> filterY;
	createGaussianDerivativeKernel(filterY, kernelsize, yorder, sigma);

	cv::sepFilter2D(input, output, input.depth(), filterX, filterY);

	return output;
}

cv::Mat_<cv::Vec3d> Derivative::normDerivativeFilter(const cv::Mat_<cv::Vec3d>& input, int order, double sigma)
{
	if (sigma <= 0) {
		return cv::Mat_<cv::Vec3d>::zeros(input.rows, input.cols);
	}

	cv::Mat_<cv::Vec3d> output(input.rows, input.cols);

	if (order == 1) {
		cv::Mat_<cv::Vec3d> imageX;
		cv::Mat_<cv::Vec3d> imageY;

		imageX = gaussianDerivativeFilter(input, 1, 0, sigma);
		imageY = gaussianDerivativeFilter(input, 0, 1, sigma);

		for (int y = 0; y < input.rows; y++) {
			for (int x = 0; x < input.cols; x++) {
				for (int i = 0; i < 3; i++) {
					output(y, x)[i] = sqrt(pow(imageX(y, x)[i], 2) + pow(imageY(y, x)[i], 2));
				}
			}
		}
	} else if (order == 2) {
		cv::Mat_<cv::Vec3d> imageXX;
		cv::Mat_<cv::Vec3d> imageXY;
		cv::Mat_<cv::Vec3d> imageYY;

		imageXX = gaussianDerivativeFilter(input, 2, 0, sigma);
		imageXY = gaussianDerivativeFilter(input, 1, 1, sigma);
		imageYY = gaussianDerivativeFilter(input, 0, 2, sigma);

		for (int y = 0; y < input.rows; y++) {
			for (int x = 0; x < input.cols; x++) {
				for (int i = 0; i < 3; i++) {
					output(y, x)[i] = sqrt(pow(imageXX(y, x)[i], 2) + (4 * pow(imageXY(y, x)[i], 2)) + pow(imageYY(y, x)[i], 2));
				}
			}
		}
	}

	return output;
}

} // namespace illumestimators
