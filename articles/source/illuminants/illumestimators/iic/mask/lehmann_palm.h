#ifndef IIC_LEHMANN_PALM_H
#define IIC_LEHMANN_PALM_H

#include <opencv2/imgproc/imgproc.hpp>

#include "mask.h"

namespace iic {

	/** Specularity segmentation according to Lehmann and Palm */
	class LehmannPalm : public Mask {
		public:
			LehmannPalm(cv::Mat_<cv::Vec3b> image, double s, double i);
			LehmannPalm(cv::Mat_<cv::Vec3s> image, double s, double i);
			cv::Mat_<uchar> getMask() const;

		protected:
	};
}

#endif // IIC_LEHMANN_PALM_H
