#ifndef ILLUMESTIMATORS_BASELINE_MEANESTIMATOR_H
#define ILLUMESTIMATORS_BASELINE_MEANESTIMATOR_H

#include "illuminantestimator.h"

namespace illumestimators {

/** The MeanEstimator provides a simple baseline illuminant estimator.
 *  It always estimates the illuminant as the mean illuminant computed
 *  on the training data.
 */
class MeanEstimator : public IlluminantEstimator
{
public:
	MeanEstimator();
	/** Constructor that sets training illuminants.
	 *  \param illuminants Vector of training illuminant chromaticities in BGR format.
	 */
	MeanEstimator(const std::vector<cv::Vec3d>& illuminants);
	~MeanEstimator();

public:
	std::string name() const;
	std::string identifier() const;

public:
	using IlluminantEstimator::estimateIlluminant;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const;

public:
	virtual void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;

	bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles);

public:
	bool save(const std::string& filename) const;
	bool load(const std::string& filename);
	virtual int error(); // dummy implementation - returns always 0

private:
	/** Mean illuminant of the training data. */
	cv::Vec3d m_mean;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_BASELINE_MEANESTIMATOR_H
