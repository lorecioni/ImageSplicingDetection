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

