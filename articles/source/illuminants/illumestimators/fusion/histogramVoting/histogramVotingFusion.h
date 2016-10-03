#ifndef ILLUM_HISTOGRAMVOTINGFUSION_H
#define ILLUM_HISTOGRAMVOTINGFUSION_H

#include "fusion/fusion.h"
#include "config_histogram_voting_fusion.h"
#include "config.h"

#include <QString>

namespace illumestimators {

class HistogramVotingFusion : public Fusion
{
public:
	HistogramVotingFusion( const ConfigHistogramVotingFusion& config );
	~HistogramVotingFusion();

public:
	std::string name() const;
	std::string identifier() const;

public:
	Illum estimateIlluminant( const cv::Mat_< cv::Vec3d >& image, const cv::Mat_<unsigned char>& mask ) const;
	Illum estimateIlluminant( const cv::Mat_< cv::Vec3d >& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask ) const;
	Illum estimateIlluminant( const std::vector< cv::Vec3d >& estimates, bool isRGB = true ) const;
	Illum estimateIlluminant( const std::vector< Illum >& estimates ) const;

public:
	void preprocessImage( const cv::Mat_< cv::Vec3d >& image, const cv::Mat_< unsigned char >& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask ) const;
public:
	bool train( const std::vector< std::string >& imageFiles, const std::vector< std::string >& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles = std::vector<std::string>() );
public:
	bool save( const std::string& filename ) const;
	bool load( const std::string& filename );
	virtual int error(); // dummy implementation - returns always 0

public:
	void saveVoting( QString output_dir = "" ) const;

private:
	ConfigHistogramVotingFusion config;
	mutable cv::Mat3i histogram; // TODO: maybe change this to cv::Mat3d (partly because of getBestBins())
	mutable cv::Mat3d smoothed_histogram;

	bool fillHistogram( const std::vector< Illum >& estimates ) const;
	bool meanSmoothing() const;
	Illum getBestEstimate( int histogram_index = 0 ) const;
	cv::Vec3i getBestBins( const cv::Mat3d& histogram, int histogram_index = 0 ) const;
};

} // namespace

#endif // ILLUM_HISTOGRAMVOTINGFUSION_H
