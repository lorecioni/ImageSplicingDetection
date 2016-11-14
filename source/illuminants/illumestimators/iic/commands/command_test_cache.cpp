#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "command_test_cache.h"
#include "iic.h"
#include "iebv.h"
#include "felzenszwalb/felzenszwalbsegmentation.h"
#include "veksler/vekslersegmentation.h"

#include "get_file.h"
#include "normal_image.h"

namespace iic {


CommandTestCache::CommandTestCache()
 : vole::Command(
		"test_cache",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de")
{
}


int CommandTestCache::execute() {
	if ((config.input_file.length() < 1) && (config.input_file_16_bit.length() < 1)) {
		std::cerr << "no input file given (-I or -J), aborted." << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec4d> iic_space;
	cv::Mat_<cv::Vec3d> normalized_image;
	if (config.input_file.length() > 0) {
		cv::Mat_<cv::Vec3b> img = cv::imread(config.input_file, 1 /* 8 bit RGB */);
		if (img.data == NULL) {
			std::cerr << "unable to read 8 bit image " << img << " (note: image must be RGB, 8 bits)" << std::endl;
			return 1;
		}
		int max = 255;
		normalized_image = vole::NormalImage::normalize(img, 0, max);
	} else {
		cv::Mat_<cv::Vec3s> img = cv::imread(config.input_file_16_bit, -1); // read as-is (16 bit, breaks if image has alpha channel)
		if (img.data == NULL) {
			std::cerr << "unable to read 16 bit image " << img << " (note: image must be RGB, 16 bits, no alpha channel)" << std::endl;
			return 1;
		}
		int max = -1;
		normalized_image = vole::NormalImage::normalize(img, 0, max);
		config.min_intensity = 0;
		config.max_intensity = max;
	}
	Iebv iebv(config);
	iebv.createIICSpace(normalized_image);

	cv::imwrite("/tmp/converted.png", normalized_image);

	int error_stat[5];
	error_stat[0] = error_stat[1] = error_stat[2] = error_stat[3] = error_stat[4] = 0;


	cv::Mat_<cv::Vec3b> color_segmented(normalized_image.rows, normalized_image.cols, cv::Vec3b(0, 0, 0));
	superpixels::VekslerSegmentation segV(40);
	if (config.input_file.length() > 0) {
		segV.addCachingInfo(config.input_file, config.input_file_16_bit, "", false);
	} else {
		segV.addCachingInfo(config.input_file, config.input_file_16_bit, "", false);
	}
//	segV.openCache();
	std::vector<superpixels::Superpixel> segmentation; // = segV.readFromCache(hashKey);
	
	segmentation = segV.superpixels((cv::Mat_<cv::Vec3d>)normalized_image);
//	if (segV.isCaching()) { segV.writeCache(); }

	return 0;

	srand(time(NULL)+rand());
	for (int i = 0; i < (int) segmentation.size(); i++) {
		std::vector<cv::Point> &coordinates = segmentation[i].coordinates;
		
//		illumestimators::Illum col = iebv.estimateIlluminant(normalized_image, coordinates);
		illumestimators::Illum col = iebv.estimateIlluminant(normalized_image, segmentation[i]);
		if (iebv.error() > 0) { error_stat[(int)(log2(iebv.error()+0.5))]++; }
		if ((col[0] < 0) || (col[1] < 0) || (col[2] < 0) || (col[0] > 1) || (col[1] > 1) || (col[2] > 1)) { col[0] = col[1] = col[2] = 0; }
		if ((col[0] < 0) || (col[1] < 0) || (col[2] < 0)) { col[0] = 1; error_stat[3]++; }
		if ((col[0] > 1) || (col[1] > 1) || (col[2] > 1)) { col[2] = 1; error_stat[4]++; }

		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			double sum = 0;
			for (int c = 0; c < 3; ++c) sum += col[c];
			for (int c = 0; c < 3; ++c)
				color_segmented[point.y][point.x][c] = col[c]*255/sum;
		}

	}
	cv::imwrite("/tmp/local_illum.png", color_segmented);
	std::cout << "local_illum written, produce output" << std::endl;
//	cv::Mat_<cv::Vec3b> segmentedV = segV.superpixelsImage(normalized_image);
//	cv::imwrite("/tmp/test_v.png", segmentedV);

	std::cout << "statistics:" << std::endl;
	std::cout << "number of segments: " << segmentation.size() << std::endl;
	std::cout << "FAILED min number pixels: " << error_stat[0] << std::endl;
	std::cout << "FAILED slope out of bounds: " << error_stat[1] << std::endl;
	std::cout << "FAILED eccentricity too low: " << error_stat[2] << std::endl;
	std::cout << "FAILED too small estimates: " << error_stat[3] << std::endl;
	std::cout << "FAILED too large estimates: " << error_stat[4] << std::endl;

	return 0;
}

void CommandTestCache::printShortHelp() const {
	std::cout << "Illuminant Estimation by Voting (modularized version)" << std::endl;
}

void CommandTestCache::printHelp() const {
	std::cout << "Illuminant Estimation by Voting (modularized version)" << std::endl;
}



}
