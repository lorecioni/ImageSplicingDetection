#ifndef ILLUMESTIMATORS_BASELINE_NOTHINGESTIMATOR_H
#define ILLUMESTIMATORS_BASELINE_NOTHINGESTIMATOR_H

#include "illuminantestimator.h"

namespace illumestimators {

/** The NothingEstimator provides a simple baseline illuminant estimator.
 *  It always estimates white as the illuminant color.
 */
class NothingEstimator : public IlluminantEstimator
{
public:
	NothingEstimator();
	~NothingEstimator();

public:
	std::string name() const;
	std::string identifier() const;

public:
	using IlluminantEstimator::estimateIlluminant;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const;

public:
	bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles);

	void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;


public:
	bool save(const std::string& filename) const;
	bool load(const std::string& filename);

	virtual int error(); // dummy implementation - returns always 0
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_BASELINE_NOTHINGESTIMATOR_H
