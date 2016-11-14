#include "error.h"

namespace illumestimators {

Error::Error(cv::Vec3d estimate, cv::Vec3d groundTruth) :
	m_estimate(estimate),
	m_groundTruth(groundTruth)
{

}

Error::~Error()
{

}

double Error::toAngularErrorDeg()
{
	return (toAngularErrorRad() * 180) / M_PI;
}

double Error::toAngularErrorRad()
{
	double normEstimate = cv::norm(m_estimate);
	double normGroundTruth = cv::norm(m_groundTruth);;

	cv::Vec3d estimate;
	cv::Vec3d groundTruth;

	estimate[0] = m_estimate[0] / normEstimate;
	estimate[1] = m_estimate[1] / normEstimate;
	estimate[2] = m_estimate[2] / normEstimate;

	groundTruth[0] = m_groundTruth[0] / normGroundTruth;
	groundTruth[1] = m_groundTruth[1] / normGroundTruth;
	groundTruth[2] = m_groundTruth[2] / normGroundTruth;

	double innerProduct = estimate[0] * groundTruth[0] + estimate[1] * groundTruth[1] + estimate[2] * groundTruth[2];

	if ((innerProduct > 1) && (innerProduct < 1.000000001)) {
		innerProduct = 1;
	}

	return acos(innerProduct);
}

double Error::toRMSE()
{
	double sumEstimate = m_estimate[0] + m_estimate[1] + m_estimate[2];
	double sumGroundTruth = m_groundTruth[0] + m_groundTruth[1] + m_groundTruth[2];

	cv::Vec3d estimate;
	cv::Vec3d groundTruth;

	estimate[0] = m_estimate[0] / sumEstimate;
	estimate[1] = m_estimate[1] / sumEstimate;
	estimate[2] = m_estimate[2] / sumEstimate;

	groundTruth[0] = m_groundTruth[0] / sumGroundTruth;
	groundTruth[1] = m_groundTruth[1] / sumGroundTruth;
	groundTruth[2] = m_groundTruth[2] / sumGroundTruth;

	double squared = 0;

	squared += pow(estimate[0] - groundTruth[0], 2);
	squared += pow(estimate[1] - groundTruth[1], 2);
	squared += pow(estimate[2] - groundTruth[2], 2);

	return sqrt(squared / 3);
}

double Error::toRgError()
{
	double sumEstimate = m_estimate[0] + m_estimate[1] + m_estimate[2];
	double sumGroundTruth = m_groundTruth[0] + m_groundTruth[1] + m_groundTruth[2];

	double squared = 0;

	squared += pow((m_estimate[2] / sumEstimate) - (m_groundTruth[2] / sumGroundTruth), 2);
	squared += pow((m_estimate[1] / sumEstimate) - (m_groundTruth[1] / sumGroundTruth), 2);

	return sqrt(squared);
}

double Error::toRError()
{
	double sumEstimate = m_estimate[0] + m_estimate[1] + m_estimate[2];
	double sumGroundTruth = m_groundTruth[0] + m_groundTruth[1] + m_groundTruth[2];

	return (m_estimate[2] / sumEstimate) - (m_groundTruth[2] / sumGroundTruth);
}

double Error::toGError()
{
	double sumEstimate = m_estimate[0] + m_estimate[1] + m_estimate[2];
	double sumGroundTruth = m_groundTruth[0] + m_groundTruth[1] + m_groundTruth[2];

	return (m_estimate[1] / sumEstimate) - (m_groundTruth[1] / sumGroundTruth);
}

} // namespace illumestimators
