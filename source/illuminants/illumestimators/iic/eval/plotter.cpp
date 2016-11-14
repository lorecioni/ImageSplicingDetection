#include "plotter.h"

#include <opencv2/highgui/highgui.hpp>

Plotter::Plotter( iic::ConfigIebv& config )
	: config_( config )
{
}

void Plotter::registerMatrix(
	cv::Mat_< cv::Vec3d > matrix,
	std::string descriptor )
{
	if( plotmap_.find( descriptor ) != plotmap_.end() ){
		if( config_.verbosity > 0 ){
			std::cout << "Warning in void Plotter::registerMatrix( cv::Mat matrix, std::string descriptor ): "
				  << "a matrix with descriptor \"" << descriptor << "\" is already registered. Skipping current matrix." << std::endl;
		}
		return;
	}

	cv::Mat_< cv::Vec3d > tmp;
	matrix.copyTo( tmp );
	plotmap_[ descriptor ] = tmp;
}

void Plotter::registerMatrix(
	cv::Mat_<cv::Vec3b> matrix,
	std::string descriptor )
{
	cv::Mat_< cv::Vec3d > d_matrix;
	matrix.convertTo( d_matrix, 8, 255 );
	return registerMatrix( d_matrix, descriptor );
}

void Plotter::addSuperpixel(
	const superpixels::Superpixel& superpixel,
	cv::Scalar& color,
	std::string descriptor )
{
	addPixels( superpixel.coordinates, color, descriptor );
}

void Plotter::addRectangle(
	const cv::Rect& rect,
	cv::Scalar& color,
	std::string descriptor )
{
	// add rectangle to all matrices
	if( descriptor == "" ){
		std::map< std::string, cv::Mat_< cv::Vec3d > >::iterator iter;
		for( ; iter != plotmap_.end(); ++iter ){
			cv::Mat_< cv::Vec3d >& mat = (*iter).second;
			cv::rectangle( mat, rect, color );
		}
	}
	// add rectangle to a specific matrix
	else{
		if( plotmap_.find( descriptor ) == plotmap_.end() ){
			if( config_.verbosity > 0 ){
				std::cout << "Warning in void Plotter::addPixels( std::vector< cv::Point > pixels, cv::Scalar& color, std::string descriptor ): "
					  << "a matrix with descriptor \"" << descriptor << "\" was not found in the plotter. "
					     "Returning without drawing anything." << std::endl;
			}
			return;
		}

		cv::Mat_< cv::Vec3d >& mat = plotmap_[ descriptor ];
		cv::rectangle( mat, rect, color );
	}
}

void Plotter::addPixels(
	std::vector< cv::Point > pixels,
	cv::Scalar& color,
	std::string descriptor )
{
	// add pixels to all matrices
	if( descriptor == "" ){
		std::map< std::string, cv::Mat_< cv::Vec3d > >::iterator iter;
		for( ; iter != plotmap_.end(); ++iter ){
			cv::Mat_< cv::Vec3d >& mat = (*iter).second;
			setTo( mat, pixels, color );
		}
	}
	// add pixels to a specific matrix
	else{
		if( plotmap_.find( descriptor ) == plotmap_.end() ){
			if( config_.verbosity > 0 ){
				std::cout << "Warning in void Plotter::addPixels( std::vector< cv::Point > pixels, cv::Scalar& color, std::string descriptor ): "
					  << "a matrix with descriptor \"" << descriptor << "\" was not found in the plotter. "
					     "Returning without drawing anything." << std::endl;
			}
			return;
		}

		cv::Mat_< cv::Vec3d >& mat = plotmap_[ descriptor ];
		setTo( mat, pixels, color );
	}
}

cv::Mat Plotter::getMatrix( std::string descriptor )
{
	return plotmap_[ descriptor ];
}

int Plotter::size() const
{
	return plotmap_.size();
}

void Plotter::drawMatrix( std::string descriptor )
{
	if( plotmap_.find( descriptor ) == plotmap_.end() ){
		if( config_.verbosity > 0 ){
			std::cout << "Warning in void Plotter::drawMatrix( std::string descriptor ) const: "
				  << "a matrix with descriptor \"" << descriptor << "\" was not found in the plotter. "
				     "Returning without drawing anything." << std::endl;
		}
		return;
	}

	std::string path = config_.output_directory;
	if( path[ path.size()-1 ] != '/' ){
		path += "/";
	}

	// final path
	path += "plot_" + descriptor + "_" + timestamp() + ".png";

	// compression
	std::vector< int > params;
	params[0] = CV_IMWRITE_PNG_COMPRESSION;
	params[1] = 9;

	cv::Mat_< cv::Vec3d >& mat = plotmap_[ descriptor ];
	cv::imwrite( path, mat );
}

void Plotter::drawMatrices()
{
	std::map< std::string, cv::Mat_< cv::Vec3d > >::iterator iter;
	for( ; iter != plotmap_.end(); ++iter ){
		drawMatrix( (*iter).first );
	}
}

////////////////////////////////
//   private helper methods   //
////////////////////////////////

void Plotter::setTo(
	cv::Mat_<cv::Vec3d>& mat,
	const std::vector< cv::Point >& coords,
	const cv::Scalar& color )
{
//	const std::vector< cv::Point >& coords = superpixel.coordinates;
	for( unsigned int i = 0; i < coords.size(); i++ ){
		mat[ coords[i].y ][ coords[i].x ][0] = color[0];
		mat[ coords[i].y ][ coords[i].x ][1] = color[1];
		mat[ coords[i].y ][ coords[i].x ][2] = color[2];
	}
}

std::string Plotter::timestamp() const
{
	time_t ltime;
	struct tm *Tm;

	ltime=time(NULL);
	Tm=localtime(&ltime);

	std::stringstream ss;
	ss << Tm->tm_year+1900
	   << "-" << Tm->tm_mon+1
	   << "-" << Tm->tm_mday
	   << "_" << Tm->tm_hour
	   <<  Tm->tm_min
	   << Tm->tm_sec;

	return ss.str();
}
