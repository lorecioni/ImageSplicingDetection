#ifndef ILLUMESTIMATORS_FUSION_AVERAGE_AVERAGEFUSION_H
#define ILLUMESTIMATORS_FUSION_AVERAGE_AVERAGEFUSION_H

#include "fusion/fusion.h"

namespace illumestimators {

class AverageFusion : public Fusion
{
public:
	enum Method { AVERAGE, LEAVE_N_OUT };

public:
	AverageFusion(const std::vector<IlluminantEstimator*>& estimators, Method method = AVERAGE, int n = 0);
	~AverageFusion();

public:
	std::string name() const;
	std::string identifier() const;

public:
	using IlluminantEstimator::estimateIlluminant;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const;
	Illum estimateIlluminant(const std::vector<cv::Vec3d>& estimates) const;

public:
	bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles = std::vector<std::string>());

	virtual void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;

public:
	bool save(const std::string& filename) const;
	bool load(const std::string& filename);
	virtual int error(); // dummy implementation - returns always 0

private:
	Method m_method;
	int m_n;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_FUSION_AVERAGE_AVERAGEFUSION_H
