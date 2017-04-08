/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_FELZENSZWALB_FELZENSWALBSEGMENTATION_H
#define SUPERPIXELS_FELZENSZWALB_FELZENSWALBSEGMENTATION_H

#include "superpixelsegmentation.h"
#include "commands/felzenszwalbconfig.h"

namespace superpixels {

class FelzenszwalbSegmentation : public SuperpixelSegmentation
{
public:
	FelzenszwalbSegmentation(vole::FelzenszwalbConfig config);
	FelzenszwalbSegmentation(int min_size, int kThreshold = 500, double sigma = 0.5);
	~FelzenszwalbSegmentation();

public:
	std::vector<Superpixel> superpixels(const cv::Mat_<cv::Vec3b>& image);
	std::vector<Superpixel> superpixels(const cv::Mat_<cv::Vec3d>& image);

private:
	vole::FelzenszwalbConfig *localCfg;
};

} // namespace superpixels

#endif // SUPERPIXELS_FELZENSZWALB_FELZENSWALBSEGMENTATION_H
