#include "nothingestimator.h"
#include "common/color.h"

namespace illumestimators {

NothingEstimator::NothingEstimator()
{

}

NothingEstimator::~NothingEstimator()
{

}

std::string NothingEstimator::name() const
{
	std::stringstream name;
	name << "NothingEstimator";

	return name.str();
}

std::string NothingEstimator::identifier() const
{
	std::stringstream identifier;
	identifier << "nothing";

	return identifier.str();
}

Illum NothingEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const
{
	return Color::calculateChromaticities(Illum(1, 1, 1));
}

Illum NothingEstimator::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	return Color::calculateChromaticities(Illum(1, 1, 1));
}

bool NothingEstimator::train(const std::vector<std::string>&, const std::vector<std::string>&, const std::vector<cv::Vec3d>&, const std::vector<std::string>&)
{
	return true;
}


void NothingEstimator::preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const
{
	outputImage = image.clone();
	outputMask = mask.clone();
}


bool NothingEstimator::save(const std::string& filename) const
{
	cv::FileStorage	fs(filename, cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		return false;
	}

	fs << "name" << "NothingEstimator";

	return true;
}

bool NothingEstimator::load(const std::string& filename)
{
	cv::FileStorage	fs(filename, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		return false;
	}

	std::string name = fs["name"];

	if (name != "NothingEstimator") {
		return false;
	}

	return true;
}

int NothingEstimator::error()
{
	return 0;
}

} // namespace illumestimators
