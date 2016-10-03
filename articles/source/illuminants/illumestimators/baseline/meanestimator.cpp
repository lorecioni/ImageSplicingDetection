#include "meanestimator.h"
#include "common/color.h"
#include "common/statistics.h"

namespace illumestimators {

MeanEstimator::MeanEstimator()
{

}

MeanEstimator::MeanEstimator(const std::vector<cv::Vec3d>& illuminants)
{
	m_mean = Statistics::mean(illuminants);
}

MeanEstimator::~MeanEstimator()
{

}

std::string MeanEstimator::name() const
{
	std::stringstream name;
	name << "MeanEstimator";

	return name.str();
}

std::string MeanEstimator::identifier() const
{
	std::stringstream identifier;
	identifier << "mean";

	return identifier.str();
}

Illum MeanEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const
{
	cv::Vec3d estimate = Color::calculateChromaticities(m_mean);
	
	return Illum(estimate[2], estimate[1], estimate[0]); // assumes bgr order in the mean
}

Illum MeanEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	cv::Vec3d estimate = Color::calculateChromaticities(m_mean);
	return Illum(estimate[2], estimate[1], estimate[0]); // assumes bgr order in the mean
}

void MeanEstimator::preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const
{
	outputImage = image.clone();
	outputMask = mask.clone();
}

bool MeanEstimator::train(const std::vector<std::string>&, const std::vector<std::string>&, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>&)
{
	m_mean = Statistics::mean(illuminants);

	return true;
}

bool MeanEstimator::save(const std::string& filename) const
{
	cv::FileStorage	fs(filename, cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		return false;
	}

	fs << "name" << "MeanEstimator";
	fs << "m_mean" << m_mean;

	return true;
}

bool MeanEstimator::load(const std::string& filename)
{
	cv::FileStorage	fs(filename, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		return false;
	}

	std::string name = fs["name"];

	if (name != "MeanEstimator") {
		return false;
	}

	cv::FileNode mean = fs["m_mean"];
	CV_Assert(mean.type() == cv::FileNode::SEQ && mean.size() == 3);
	m_mean = cv::Vec3d((double) mean[0], (double) mean[1], (double) mean[2]);

	return true;
}

int MeanEstimator::error()
{
	return 0;
}


} // namespace illumestimators
