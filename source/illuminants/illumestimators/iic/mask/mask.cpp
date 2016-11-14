#include <cassert>

#include "mask.h"

namespace iic {

Mask::Mask()
{
	valid = false;
}

// full image mask overlay
void Mask::addMask(const Mask &mask2, const MASKING_OPERATION op) {
	assert(valid == true);
//	assert(mask.rows == the_mask2.rows);
//	assert(mask.cols == the_mask2.cols);

	addMask(mask2, cv::Vec2i(0, 0), op);
}

// mask overlay - no bounding box checking
void Mask::addMask(const Mask &mask2, const cv::Point pos, const MASKING_OPERATION op) {

	const cv::Mat_<uchar> &the_mask2 = mask2.getMask();
	if ((pos.x >= mask.cols) || (pos.y >= mask.rows)) return;

	int maxX = std::min<int>(pos.x + the_mask2.cols, mask.cols);
	int maxY = std::min<int>(pos.y + the_mask2.rows, mask.rows);

	switch (op) {
		AND:
			for (int y = pos.y; y < maxY; ++y) 
				for (int x = pos.x; x < maxX; ++x) 
					mask[y][x] = (mask[y][x] > 0 && the_mask2[y][x] > 0) ? 255 : 0;
			break;
		OR:
			for (int y = pos.y; y < maxY; ++y) 
				for (int x = pos.x; x < maxX; ++x) 
					mask[y][x] = (mask[y][x] > 0 || the_mask2[y][x] > 0) ? 255 : 0;
			break;
		SATURATE_ADD:
			for (int y = pos.y; y < maxY; ++y) 
				for (int x = pos.x; x < maxX; ++x) 
					mask[y][x] = (uchar)std::min<int>(255, (int)mask[y][x] + (int)the_mask2[y][x]);
			break;
		NORMALIZED_MULTIPLY:
			for (int y = pos.y; y < maxY; ++y) 
				for (int x = pos.x; x < maxX; ++x) 
					mask[y][x] = (uchar)(((mask[y][x]/255.0) * (the_mask2[y][x]/255.0))*255.0);
			break;
		default:
			assert(1337==0xDeadBabe);
	}
}

}
