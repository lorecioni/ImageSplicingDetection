/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef LILLE_PAINT_SUPERPIXEL_H
#define LILLE_PAINT_SUPERPIXEL_H

#include "superpixel.h"
#include "color.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace lille {
	class PaintSuperpixel {
		public:
		static void paint(cv::Mat_<cv::Vec3b> &targetImg, superpixels::Superpixel &s, rbase::Color &col);
	};
}

#endif // LILLE_PAINT_SUPERPIXEL_H
