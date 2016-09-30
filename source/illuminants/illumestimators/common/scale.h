#ifndef ILLUMESTIMATORS_COMMON_SCALE_H
#define ILLUMESTIMATORS_COMMON_SCALE_H

#include <opencv2/core/core.hpp>
#include <vector>

#include "common/statistics.h"

namespace illumestimators {

class Scale
{
public:
	static void scaleByMax(cv::Mat_<cv::Vec3d>& image) {
		double max = Statistics::max(Statistics::max(image));

		for (int y = 0; y < image.rows; y++) {
			for (int x = 0; x < image.cols; x++) {
				for (int i = 0; i < 3; i++) {
					image(y,x)[i] /= max;
				}
			}
		}
	}

	static void scaleByMax(std::vector<cv::Vec3d>& pixels) {
		double max = Statistics::max(Statistics::max(pixels));

		for (unsigned int i = 0; i < pixels.size(); i++) {
			pixels[i][0] /= max;
			pixels[i][1] /= max;
			pixels[i][2] /= max;
		}
	}
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMON_SCALE_H
