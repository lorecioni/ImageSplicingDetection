#ifndef ILLUMESTIMATORS_EVALUATION_ERROR_H
#define ILLUMESTIMATORS_EVALUATION_ERROR_H

#include <opencv2/core/core.hpp>

namespace illumestimators {

class Error
{
public:
	Error(cv::Vec3d estimate, cv::Vec3d groundTruth);
	~Error();

public:
	double toAngularErrorDeg();
	double toAngularErrorRad();
	double toRgError();
	double toRError();
	double toGError();
	double toRMSE();

private:
	cv::Vec3d m_estimate;
	cv::Vec3d m_groundTruth;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_EVALUATION_ERROR_H
