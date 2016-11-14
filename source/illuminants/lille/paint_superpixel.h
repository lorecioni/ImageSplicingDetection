#ifndef LILLE_PAINT_SUPERPIXEL_H
#define LILLE_PAINT_SUPERPIXEL_H

#include "superpixel.h"
#include "color.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace lille {
	class PaintSuperpixel {
		public:
		static void paint(cv::Mat_<cv::Vec3b> &targetImg, superpixels::Superpixel &s, rbase::Color &col);
	};
}

#endif // LILLE_PAINT_SUPERPIXEL_H
