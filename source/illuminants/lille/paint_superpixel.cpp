/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "paint_superpixel.h"

namespace lille {

void PaintSuperpixel::paint(cv::Mat_<cv::Vec3b> &targetImg, superpixels::Superpixel &s, rbase::Color &col)
{
	for (std::vector<cv::Point>::iterator c = s.coordinates.begin(); c != s.coordinates.end(); c++) {
		targetImg[c->y][c->x][2] = col.r * 255;
		targetImg[c->y][c->x][1] = col.g * 255;
		targetImg[c->y][c->x][0] = col.b * 255;
	}
}


}

