/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "felzenszwalbsegmentation.h"
#include "implementation/segment-image.h"
#include "implementation/image.h"
#include "implementation/misc.h"
#include "commands/felzenszwalbconfig.h"
#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>


namespace superpixels
{

FelzenszwalbSegmentation::FelzenszwalbSegmentation(vole::FelzenszwalbConfig config)
	: SuperpixelSegmentation(std::string("felzenszwalb"))
{
	localCfg = new vole::FelzenszwalbConfig();

	localCfg->input_file  = config.input_file;
	localCfg->input_file_16_bit  = config.input_file_16_bit;
	localCfg->min_intensity = config.min_intensity;
	localCfg->max_intensity = config.max_intensity;
	localCfg->output_file = config.output_file;
	localCfg->isGraphical = config.isGraphical;
	localCfg->kThreshold  = config.kThreshold;
	localCfg->sigma       = config.sigma;
	localCfg->min_size    = config.min_size;

	this->config = localCfg;
}

FelzenszwalbSegmentation::FelzenszwalbSegmentation(int min_size, int kThreshold, double sigma) :
	SuperpixelSegmentation(std::string("felzenszwalb"))
{
	localCfg = new vole::FelzenszwalbConfig();

	localCfg->input_file   = ""; 
	localCfg->input_file_16_bit = "";
	localCfg->min_intensity = 0;
	localCfg->max_intensity = 1;
	localCfg->output_file = "";
	localCfg->isGraphical = false;
	localCfg->kThreshold  = kThreshold;
	localCfg->sigma       = sigma;
	localCfg->min_size    = min_size;
	
	this->config = localCfg;
}

FelzenszwalbSegmentation::~FelzenszwalbSegmentation()
{
	// superclass is deleting config in its destructor, hence also localCfg. We don't need to delete here. Weird/ugly but true.
}

std::vector<Superpixel> FelzenszwalbSegmentation::superpixels(const cv::Mat_<cv::Vec3b>& image)
{
	height = image.rows;
	width  = image.cols;

	felzenszwalb::image<felzenszwalb::rgb>* inputImage = new felzenszwalb::image<felzenszwalb::rgb>(width, height);

	for (int y = 0; y < height; y++) {
		for (int  x = 0; x < width; x++) {
			imRef(inputImage, x, y).r = image(y, x)[2];
			imRef(inputImage, x, y).g = image(y, x)[1];
			imRef(inputImage, x, y).b = image(y, x)[0];
		}
	}

	int numSegments;
	felzenszwalb::image<felzenszwalb::rgb>* segmentation
		= felzenszwalb::segment_image(inputImage,
			localCfg->sigma,
			localCfg->kThreshold,
			localCfg->min_size,
			&numSegments);

	_superpixels = std::vector<Superpixel>(numSegments);
	std::vector<Superpixel>::iterator sup;
	for (sup = _superpixels.begin(); sup != _superpixels.end(); sup++) {
		sup->bbox = cv::Rect(INT_MAX, INT_MAX, INT_MIN, INT_MIN);
	}

	std::map<int, int> labels;
	int label = 0;

	for (int y = 0; y < height; y++) {
		for (int  x = 0; x < width; x++) {
			cv::Point coord(x, y);
			int value = (imRef(segmentation, x, y).r << 16) + (imRef(segmentation, x, y).g << 8) + imRef(segmentation, x, y).b;

			if (labels.find(value) == labels.end()) {
				if (label >= numSegments) {
					std::cerr << "Error in felzenszwalb superpixel segmentation!";
					_superpixels.clear();
					labels.clear();
					break;
				}

				labels[value] = label;
				label++;
			}

			Superpixel &sup = _superpixels[labels[value]];
			sup.coordinates.push_back(coord);
			sup.bbox.x      = std::min(sup.bbox.x,      coord.x);
			sup.bbox.width  = std::max(sup.bbox.width,  coord.x);
			sup.bbox.y      = std::min(sup.bbox.y,      coord.y);
			sup.bbox.height = std::max(sup.bbox.height, coord.y);
		}
	}

	for (sup = _superpixels.begin(); sup != _superpixels.end(); sup++) {
		sup->bbox.width  = sup->bbox.width  - sup->bbox.x + 1;
		sup->bbox.height = sup->bbox.height - sup->bbox.y + 1;
	}

	delete inputImage;
	delete segmentation;

	return _superpixels;
}

std::vector<Superpixel> FelzenszwalbSegmentation::superpixels(const cv::Mat_<cv::Vec3d>& image)
{
	cv::Mat_<cv::Vec3b> inputImage;
	image.convertTo(inputImage, 8, 255);
	return superpixels(inputImage);
}


} // namespace superpixels
