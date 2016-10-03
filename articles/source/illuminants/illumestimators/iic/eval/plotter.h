#ifndef PLOTTER_H
#define PLOTTER_H

#include <opencv2/core/core.hpp>

#include "superpixel.h"
#include "config_iebv.h"

class Plotter
{
public:
	Plotter( iic::ConfigIebv& config );

	void registerMatrix( cv::Mat_< cv::Vec3d > matrix, std::string descriptor );
	void registerMatrix( cv::Mat_< cv::Vec3b > matrix, std::string descriptor );
public:
	void addSuperpixel( const superpixels::Superpixel& superpixel, cv::Scalar& color,  std::string descriptor = "" );
	void addRectangle( const cv::Rect& rect, cv::Scalar& color, std::string descriptor = "" );
	void addPixels( std::vector< cv::Point > pixels, cv::Scalar& color, std::string descriptor = "" );

public:
	cv::Mat getMatrix( std::string descriptor );
	int size() const;

public:
	void drawMatrix( std::string descriptor );
	void drawMatrices();

private:
	std::map< std::string, cv::Mat_< cv::Vec3d > > plotmap_;
	iic::ConfigIebv& config_;

private:
	void setTo( cv::Mat_< cv::Vec3d >& mat, const std::vector< cv::Point >& coords, const cv::Scalar& color );
	std::string timestamp() const;
};

#endif // PLOTTER_H
