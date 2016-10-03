#include "averagefusion.h"
#include "evaluation/error.h"
#include "common/statistics.h"
#include <limits>
#include <iostream>

namespace illumestimators {

AverageFusion::AverageFusion(const std::vector<IlluminantEstimator*>& estimators, Method method, int n) :
	Fusion(estimators),
	m_method(method),
	m_n(n)
{

}

AverageFusion::~AverageFusion()
{
}

std::string AverageFusion::name() const
{
	std::stringstream name;

	if (m_method == AVERAGE) {
		name << "AverageFusion(method = average)";
	} else if (m_method == LEAVE_N_OUT) {
		name << "AverageFusion(method = leave_n_out, n = " << m_n << ")";
	}

	return name.str();
}

std::string AverageFusion::identifier() const
{
	std::stringstream identifier;

	identifier << "averagefusion-" << m_method << "-" << m_n;

	return identifier.str();
}

Illum AverageFusion::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const
{
	std::vector<IlluminantEstimator*> estimators = getEstimators();
	std::vector<cv::Vec3d> estimates;

	for (unsigned int i = 0; i < estimators.size(); i++) {
		Illum estimate = estimators[i]->estimateIlluminant(image, mask);
		estimates.push_back(cv::Vec3d(estimate.b, estimate.g, estimate.r)); // assuming bgr order
	}

	return estimateIlluminant(estimates);
}

Illum AverageFusion::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	return Illum();
}

Illum AverageFusion::estimateIlluminant(const std::vector<cv::Vec3d>& estimates) const
{
	cv::Vec3d result;
	std::vector<cv::Vec3d> inputEstimates = estimates;

	if (m_method == AVERAGE) {
		result = Statistics::mean(inputEstimates);
	} else if (m_method == LEAVE_N_OUT) {
		std::vector<double> distances;

		for (unsigned int i = 0; i < inputEstimates.size(); i++) {
			double distance = 0;
			for (unsigned int j = 0; j < inputEstimates.size(); j++) {
				distance += Error(inputEstimates[i], inputEstimates[j]).toRgError();
			}
			distances.push_back(distance);
		}

		for (int n = 0; n < m_n; n++) {
			if (inputEstimates.size() < 2) {
				break;
			}

			int index = 0;
			double maxDistance = -std::numeric_limits<double>::max();
			for (unsigned int i = 0; i < inputEstimates.size(); i++) {
				if (distances[i] > maxDistance) {
					index = i;
					maxDistance = distances[i];
				}
			}

			distances.erase(distances.begin() + index);
			inputEstimates.erase(inputEstimates.begin() + index);
		}

		result = Statistics::mean(inputEstimates);
	}

	return Illum(result[2], result[1], result[0]); // assuming bgr order
}

bool AverageFusion::train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles)
{
	return true;
}

bool AverageFusion::save(const std::string& filename) const
{
	cv::FileStorage	fs(filename, cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		return false;
	}

	fs << "name" << "AverageFusion";
	fs << "method" << m_method;
	fs << "n" << m_n;

	return true;
}

bool AverageFusion::load(const std::string& filename)
{
	cv::FileStorage	fs(filename, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		return false;
	}

	std::string name = fs["name"];

	if (name != "AverageFusion") {
		return false;
	}

	m_method = (Method) (int) fs["method"];
	m_n = fs["n"];

	return true;
}

int AverageFusion::error()
{
	return 0;
}


} // namespace illumestimators
