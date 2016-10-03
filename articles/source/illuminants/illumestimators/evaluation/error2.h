#ifndef ILLUMESTIMATORS_EVALUATION_ERROR2_H
#define ILLUMESTIMATORS_EVALUATION_ERROR2_H

#include <opencv2/core/core.hpp>

namespace illumestimators {

class Error2
{
public:
	Error2();
	~Error2();

public:
	static double toAngularErrorDeg(cv::Vec3d &estimate, cv::Vec3d &groundTruth);
	static double toAngularErrorRad(cv::Vec3d &estimate, cv::Vec3d &groundTruth);
	static double toRgError(cv::Vec3d &estimate, cv::Vec3d &groundTruth);
	static double toRError(cv::Vec3d &estimate, cv::Vec3d &groundTruth);
	static double toGError(cv::Vec3d &estimate, cv::Vec3d &groundTruth);
	static double toRMSE(cv::Vec3d &estimate, cv::Vec3d &groundTruth);

private:
//	cv::Vec3d m_estimate;
//	cv::Vec3d m_groundTruth;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_EVALUATION_ERROR2_H
