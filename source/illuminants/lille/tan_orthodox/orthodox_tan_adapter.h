#ifndef LILLE_ORTHODOX_TAN_ADAPTER_H
#define LILLE_ORTHODOX_TAN_ADAPTER_H

#include <opencv2/imgproc/imgproc.hpp>
#include "image.h"
#include "pnmfile.h"
#include "tan_misc.h"
#include "zGlobal.h"
#include "illum.h"

namespace lille {

class OrthodoxTanAdapter {
public:
	OrthodoxTanAdapter();

	illumestimators::Illum estimate(cv::Mat_<cv::Vec3b> image);
	illumestimators::Illum estimate(cv::Mat_<cv::Vec3b> image, cv::Mat_<unsigned char> mask);

protected:

};

}

#endif // LILLE_ORTHODOX_TAN_ADAPTER_H
