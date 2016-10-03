/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef IIC_IEBV_ESTIMATOR_H
#define IIC_IEBV_ESTIMATOR_H

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

#include "illum.h"
#include "config_iebv.h"
#include "superpixel.h"

namespace iic {

class IebvEstimator {
public:
	IebvEstimator(const ConfigIebv &config);
	
	illumestimators::Illum estimate(const cv::Mat_<cv::Vec4d> &iic_space, const superpixels::Superpixel& superpixel);

	int error();

	const ConfigIebv &config;

protected:

	bool error_min_number_pixels;
	bool error_slope_out_of_bounds;
	bool error_eccentricity_too_low;

};

}

#endif // IIC_IEBV_ESTIMATOR_H
