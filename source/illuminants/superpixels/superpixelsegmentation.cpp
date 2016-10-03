/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "superpixelsegmentation.h"
#include "color.h"
#include <opencv2/highgui/highgui.hpp>
#include <boost/foreach.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <sstream>

// boost property_tree exists only from boost 1.41 on; don't support earlier
// versions, as even debian squeeze contains boost >= 1.41
#if BOOST_VERSION >= 104100
#define SUPERPIXEL_SEGMENTATION_WITH_CACHING
#endif 

#ifdef SUPERPIXEL_SEGMENTATION_WITH_CACHING
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#endif // SUPERPIXEL_SEGMENTATION_WITH_CACHING

namespace superpixels {

SuperpixelSegmentation::SuperpixelSegmentation(std::string descriptor, int height, int width) :
	height(height), width(width)
{
	config = NULL;
	caching_activated = false;
}

SuperpixelSegmentation::SuperpixelSegmentation( const SuperpixelSegmentation& cpy )
	: height( cpy.height ), width( cpy.width ), _superpixels( cpy._superpixels ), label_image_( cpy.label_image_ )
{}

SuperpixelSegmentation::~SuperpixelSegmentation()
{
	delete config;
	config = 0;
}

cv::Mat_<cv::Vec3b> SuperpixelSegmentation::superpixelsImage(const cv::Mat_<cv::Vec3b>& image)
{
	return superpixelsImage(superpixels(image), image.rows, image.cols);
}

cv::Mat_<cv::Vec3b> SuperpixelSegmentation::superpixelsImage(const cv::Mat_<cv::Vec3d>& image)
{
	return superpixelsImage(superpixels(image), image.rows, image.cols);
}

cv::Mat_<int> SuperpixelSegmentation::superpixelsLabelImage(const cv::Mat_<cv::Vec3b>& image)
{
	return superpixelsLabelImage(superpixels(image), image.rows, image.cols);
}

cv::Mat_<int> SuperpixelSegmentation::superpixelsLabelImage(const cv::Mat_<cv::Vec3d>& image)
{
	return superpixelsLabelImage(superpixels(image), image.rows, image.cols);
}

vole::Config *SuperpixelSegmentation::getConfig()
{
	return config;
}

cv::Mat_<cv::Vec3b> SuperpixelSegmentation::superpixelsImage(
	const std::vector<Superpixel>& superpixels,
	int rows,
	int cols,
	bool collisionFree)
{
	cv::Mat_<cv::Vec3b> superpixelsImage(rows, cols);

	rbase::Color c;
	for (unsigned int i = 0; i < superpixels.size(); i++) {
		Superpixel superpixel = superpixels[i];

		if (!collisionFree) {
			// NOTE: Assumes that srand has been called!
			c.initRandomly();
		} else {
			c.initFromId(i);
		}

		for (unsigned int j = 0; j < superpixel.coordinates.size(); j++) {
			cv::Point coord = superpixel.coordinates[j];

			superpixelsImage(coord.y, coord.x)[0] = c.r;
			superpixelsImage(coord.y, coord.x)[1] = c.g;
			superpixelsImage(coord.y, coord.x)[2] = c.b;
		}
	}

	return superpixelsImage;
}

cv::Mat_< int > & SuperpixelSegmentation::superpixelsLabelImage(
	const std::vector<Superpixel>& superpixels,
	int rows,
	int cols )
{
	if( !label_image_.empty() ){
		return label_image_;
	}

//	label_image_ = cv::Mat_<int>(rows, cols);
	label_image_.create( rows, cols );

	for( unsigned int i = 0; i < superpixels.size(); i++ ){
		for( unsigned int j = 0; j < superpixels[i].coordinates.size(); j++ ){
			cv::Point coord = superpixels[i].coordinates[j];
			// we can use i as our label number
			label_image_(coord.y, coord.x) = i;
		}
	}

	return label_image_;
}

// FIXME inefficient implementation!
void SuperpixelSegmentation::imageToSuperpixels(
	cv::Mat_<cv::Vec3b> &image,
	std::vector<Superpixel>& superpixels)
{
	std::map<unsigned long int, int> index;
	superpixels.clear();
	int count = 0;
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; ++x) {
			unsigned long int cur = rbase::Color::rgbToId(image[y][x][2], image[y][x][1], image[y][x][0]);
			if (index.find(cur) == index.end()) {
				index[cur] = count;
				superpixels.push_back(Superpixel());
				count++;
			}
			superpixels[index[cur]].coordinates.push_back(cv::Point(x, y));
		}
	}
}

} // namespace superpixels
