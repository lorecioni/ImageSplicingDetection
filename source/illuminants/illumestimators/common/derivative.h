/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef ILLUMESTIMATORS_COMMON_DERIVATIVE_H
#define ILLUMESTIMATORS_COMMON_DERIVATIVE_H

#include <opencv2/core/core.hpp>

namespace illumestimators {

/** The Derivative class provides a derivative filter*/
class Derivative
{
public:
	/** Apply a Gaussian derivative filter
	  * \param input Input image
	  * \param xorder Derivative order in x direction
	  * \param yorder Derivative order in y direction
	  * \param sigma Standard deviation of Gaussian smoothing filter
	  * \return Filtered image
	  */
	static cv::Mat_<cv::Vec3d> gaussianDerivativeFilter(const cv::Mat_<cv::Vec3d>& input, int xorder, int yorder, double sigma);
	/** Apply a norm derivative filter
	  * \param input Input image
	  * \param order Derivative order
	  * \param sigma Standard deviation of Gaussian smoothing filter
	  * \return Filtered image
	  */
	static cv::Mat_<cv::Vec3d> normDerivativeFilter(const cv::Mat_<cv::Vec3d>& input, int order, double sigma);

private:
	static double gaussianDerivativeFunction(double x, int order, double sigma);
	static void createGaussianDerivativeKernel(cv::Mat_<double>& filter, int kernelsize, int order, double sigma);
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMON_DERIVATIVE_H
