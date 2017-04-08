/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef ILLUM_HISTOGRAMVOTINGFUSION_H
#define ILLUM_HISTOGRAMVOTINGFUSION_H

#include "fusion/fusion.h"
#include "config_histogram_voting_fusion.h"
#include "config.h"

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
	virtual int error(); // dummy implementation - returns always 0

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
