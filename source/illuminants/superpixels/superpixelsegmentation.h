/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_SUPERPIXELSEGMENTATION_H
#define SUPERPIXELS_SUPERPIXELSEGMENTATION_H

#include "superpixel.h"
#include "config.h"
#include <opencv2/core/core.hpp>
#include <sstream>

namespace superpixels {

/** The SuperpixelSegmentation class provides a base class for superpixel segmentation algorithms. */
class SuperpixelSegmentation 
{
public:
	SuperpixelSegmentation(std::string descriptor = "", int height = -1, int width = -1);
	SuperpixelSegmentation( const SuperpixelSegmentation& to_copy );
	virtual ~SuperpixelSegmentation();

public:
	vole::Config *getConfig();

public:
	/** Segment an image into superpixels.
	 *  \param image Image in BGR format.
	 *  \return Vector of superpixels.
	 */
	virtual std::vector<Superpixel> superpixels(const cv::Mat_<cv::Vec3b>& image) = 0;
	/** Segment an image into superpixels.
	 *  \param image Image in BGR format.
	 *  \return Vector of superpixels.
	 */
	virtual std::vector<Superpixel> superpixels(const cv::Mat_<cv::Vec3d>& image) = 0;
	/** Create an image with superpixels marked by random colors.
	 *  \param image Image in BGR format.
	 *  \return Image with superpixels marked by random colors.
	 */
	cv::Mat_<cv::Vec3b> superpixelsImage(const cv::Mat_<cv::Vec3b>& image);
	/** Create an image with superpixels marked by random colors.
	 *  \param image Image in BGR format.
	 *  \return Image with superpixels marked by random colors.
	 */
	cv::Mat_<cv::Vec3b> superpixelsImage(const cv::Mat_<cv::Vec3d>& image);
	/** Create an image with superpixels marked by integer label numbers.
	 *  \param image Image in BGR format.
	 *  \return Image with superpixels marked by integer label numbers.
	 */
	cv::Mat_<int> superpixelsLabelImage(const cv::Mat_<cv::Vec3b>& image);
	/** Create an image with superpixels marked by integer label numbers.
	 *  \param image Image in BGR format.
	 *  \return Image with superpixels marked by integer label numbers.
	 */
	cv::Mat_<int> superpixelsLabelImage(const cv::Mat_<cv::Vec3d>& image);

	/* TODO damit Svens Code diese Klasse auch benutzen kann:
	 * virtual std::vector<Superpixel> superpixelsWithMask(const cv::Mat_<cv::Vec3b>& image) = 0;
	 */
 
public:
	/** Create an image with superpixels marked by random colors.
	 *  \param superpixels Vector of superpixels.
	 *  \param rows Number of rows of the image
	 *  \param cols Number of columns of the image
	 *  \return Image with superpixels marked by random colors.
	 */
	// TODO: comment on parameter collisionFree!
	// NOTE: Assumes that srand has been called if collisionFree == false
	cv::Mat_<cv::Vec3b> superpixelsImage(
		const std::vector<Superpixel>& superpixels,
		int rows,
		int cols,
		bool collisionFree = false);

	/** Create an image with superpixels marked by integer label numbers.
	 *  \param superpixels Vector of superpixels.
	 *  \param rows Number of rows of the image
	 *  \param cols Number of columns of the image
	 *  \return Image with superpixels marked by integer label numbers.
	 */
	cv::Mat_<int> & superpixelsLabelImage(const std::vector<Superpixel>& superpixels, int rows, int cols);

	// TODO: comment function!
	static void imageToSuperpixels(cv::Mat_<cv::Vec3b> &image, std::vector<Superpixel>& superpixels);

	/** Get the number of segments.
	 *  \return Number of segments, i.e. size of superpixels vector.
	 */
	int sizeSuperpixels() const { return _superpixels.size(); }

protected:
	vole::Config *config;

	// TODO: cleanup: why are height and width in this superclass? They fit more to
	//       patches than superpixels. And superpixels::Superpixel::bbox already 
	//       offers something similar.
	int height, width;
	std::vector<Superpixel> _superpixels;
	cv::Mat_<int> label_image_;

	bool caching_activated;

};

} // namespace superpixels

#endif // SUPERPIXELS_SUPERPIXELSEGMENTATION_H
