/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_SUPERPIXEL_H
#define SUPERPIXELS_SUPERPIXEL_H

#include <opencv2/core/core.hpp>

namespace superpixels {

/** Representation of a superpixel. */
class Superpixel {
public:
	Superpixel()
		: size( 0 )
	{}

	/** Coordinates of the pixels in the image belonging to the superpixel. */
	std::vector<cv::Point> coordinates;
	cv::Rect bbox;
	int size;
	
	void clear()
	{
		coordinates.clear();
		bbox.x = 0;
		bbox.y = 0;
		bbox.width = 0;
		bbox.height = 0;
		size = 0;
	}
	
	bool empty() const
	{
		return (size == 0 ? true : false);
	}
	
};

} // namespace superpixels

#endif // SUPERPIXELS_SUPERPIXEL_H
