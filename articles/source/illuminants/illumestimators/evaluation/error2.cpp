#include "error2.h"

namespace illumestimators {

Error2::Error2()
{

}

Error2::~Error2()
{

}

double Error2::toAngularErrorDeg(cv::Vec3d &estimate, cv::Vec3d &groundTruth)
{
	return (toAngularErrorRad(estimate, groundTruth) * 180) / M_PI;
}

double Error2::toAngularErrorRad(cv::Vec3d &the_estimate, cv::Vec3d &the_groundTruth)
{
	cv::Vec3d estimate = the_estimate;
	cv::Vec3d groundTruth = the_groundTruth;

	double normEstimate = cv::norm(estimate);
	double normGroundTruth = cv::norm(groundTruth);;

	estimate[0] = the_estimate[0] / normEstimate;
	estimate[1] = the_estimate[1] / normEstimate;
	estimate[2] = the_estimate[2] / normEstimate;

	groundTruth[0] = the_groundTruth[0] / normGroundTruth;
	groundTruth[1] = the_groundTruth[1] / normGroundTruth;
	groundTruth[2] = the_groundTruth[2] / normGroundTruth;

	double innerProduct = estimate[0] * groundTruth[0] + estimate[1] * groundTruth[1] + estimate[2] * groundTruth[2];

	if ((innerProduct > 1) && (innerProduct < 1.000000001)) {
		innerProduct = 1;
	}

	return acos(innerProduct);
}

double Error2::toRMSE(cv::Vec3d &the_estimate, cv::Vec3d &the_groundTruth)
{
	cv::Vec3d estimate = the_estimate;
	cv::Vec3d groundTruth = the_groundTruth;

	double sumEstimate = estimate[0] + estimate[1] + estimate[2];
	double sumGroundTruth = groundTruth[0] + groundTruth[1] + groundTruth[2];

	estimate[0] = estimate[0] / sumEstimate;
	estimate[1] = estimate[1] / sumEstimate;
	estimate[2] = estimate[2] / sumEstimate;

	groundTruth[0] = groundTruth[0] / sumGroundTruth;
	groundTruth[1] = groundTruth[1] / sumGroundTruth;
	groundTruth[2] = groundTruth[2] / sumGroundTruth;

	double squared = 0;

	squared += pow(estimate[0] - groundTruth[0], 2);
	squared += pow(estimate[1] - groundTruth[1], 2);
	squared += pow(estimate[2] - groundTruth[2], 2);

	return sqrt(squared / 3);
}

double Error2::toRgError(cv::Vec3d &estimate, cv::Vec3d &groundTruth)
{
	double sumEstimate = estimate[0] + estimate[1] + estimate[2];
	double sumGroundTruth = groundTruth[0] + groundTruth[1] + groundTruth[2];

	double squared = 0;

	squared += pow((estimate[2] / sumEstimate) - (groundTruth[2] / sumGroundTruth), 2);
	squared += pow((estimate[1] / sumEstimate) - (groundTruth[1] / sumGroundTruth), 2);

	return sqrt(squared);
}

double Error2::toRError(cv::Vec3d &estimate, cv::Vec3d &groundTruth)
{
	double sumEstimate = estimate[0] + estimate[1] + estimate[2];
	double sumGroundTruth = groundTruth[0] + groundTruth[1] + groundTruth[2];

	return (estimate[2] / sumEstimate) - (groundTruth[2] / sumGroundTruth);
}

double Error2::toGError(cv::Vec3d &estimate, cv::Vec3d &groundTruth)
{
	double sumEstimate = estimate[0] + estimate[1] + estimate[2];
	double sumGroundTruth = groundTruth[0] + groundTruth[1] + groundTruth[2];

	return (estimate[1] / sumEstimate) - (groundTruth[1] / sumGroundTruth);
}

} // namespace illumestimators
