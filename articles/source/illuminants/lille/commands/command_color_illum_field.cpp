#include <iostream>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>

#include "commands/command_color_illum_field.h"
#include "iic.h"
#include "iebv.h"
#include "tan.h"
#include "felzenszwalb/felzenszwalbsegmentation.h"
#include "patches/patchessegmentation.h"
#include "veksler/vekslersegmentation.h"
#include "illuminantestimator.h"
#include "img_read.h"
#include "fusion/histogramVoting/histogramVotingFusion.h"
#include "plotter.h"

namespace lille {


CommandColorIllumField::CommandColorIllumField()
 : vole::Command(
		"lcolfield",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de")
{
}

int CommandColorIllumField::execute( illumestimators::Illum& estimate )
{
	int ret = execute();
	estimate = estimate_;
	return ret;
}

int CommandColorIllumField::execute() {
	if (config.verbosity > 0) std::cout << "Coloring Local Illuminant Estimates" << std::endl;

	// guarantee that all verbosities are equivalent.
//	if (config.verbosity != config.seg_felz.verbosity) {
//		config.seg_felz.verbosity = config.verbosity;
//	}
//	if (config.verbosity != config.seg_patches.verbosity) {
//		config.seg_patches.verbosity = config.verbosity;
//	}

	if ((config.input_file.length() < 1) && (config.input_file_16_bit.length() < 1)) {
		std::cerr << "no input file given (-I or -J), aborted." << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec3d> normalized_image;
	int max_int = config.input_intensity_max;
	if (config.input_file.length() > 0) {
		normalized_image = iread::IRead::img_read_dl_max(config.input_file, "rgb", 0, max_int, 1);
	} else {
		normalized_image = iread::IRead::img_read_dl_max(config.input_file_16_bit, "rgb", 0, max_int, 1);
	}
	std::cout << "max intensity: " << max_int << std::endl;

	illumestimators::IlluminantEstimator *estimator;
	if (config.with_hough) {
		iic::Tan *t = new iic::Tan(config);
		t->createIICSpace(normalized_image);
		estimator = t;
	} else {
		iic::Iebv *i = new iic::Iebv(config);
		i->createIICSpace(normalized_image);
		estimator = i;
	}

	std::string img_name = config.input_file_16_bit;
	if (config.input_file_16_bit.find_last_of('/') != std::string::npos) {
		img_name = img_name.substr(config.input_file_16_bit.find_last_of('/')+1);
	}
	if (img_name.find_last_of('.') != std::string::npos) {
		img_name = img_name.substr(0, img_name.find_last_of('.'));
	}


	std::string outfile;
	{ std::stringstream s; s << config.output_directory << "/iic_import_" << img_name << "_" << config.input_intensity_max << ".png"; outfile = s.str(); }
	cv::imwrite(outfile, normalized_image*255);

	cv::Mat_<cv::Vec3b> color_segmented(normalized_image.rows, normalized_image.cols, cv::Vec3b(0, 0, 0));
	superpixels::VekslerSegmentation segV(config.seg_veks);
	segV.addCachingInfo(config.input_file, config.input_file_16_bit);
	std::vector<superpixels::Superpixel> segmentation = segV.superpixels((cv::Mat_<cv::Vec3d>)normalized_image);
	srand(time(NULL)+rand());

	cv::Mat_<unsigned char> confidence(normalized_image.rows, normalized_image.cols, (unsigned char)0);
	int overall_max = 0;

	for (int i = 0; i < (int) segmentation.size(); i++) {
		std::vector<cv::Point> &coordinates = segmentation[i].coordinates;

		// confidence
		double max = 0;
		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			for (int c = 0; c < 3; ++c)
				if (normalized_image[point.y][point.x][c] > max)
					max = normalized_image[point.y][point.x][c];
		}
		if (max > overall_max) overall_max = max;
		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			confidence[point.y][point.x] = max;
		}
		
		illumestimators::Illum col = estimator->estimateIlluminant(normalized_image, segmentation[i]);

		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			double sum = 0;
			for (int c = 0; c < 3; ++c) sum += col[c];
			if (fabs(sum) <= std::numeric_limits<double>::epsilon()) sum = 1.0/255;
			for (int c = 0; c < 3; ++c)
				color_segmented[point.y][point.x][c] = col[c]*255/sum;
		}
	}
	std::cout << "interesting: overall_max = " << overall_max << std::endl;

	// confidence
	if (overall_max <= std::numeric_limits<double>::epsilon()) overall_max = 1.0/255;
	for (int y = 0; y < confidence.rows; ++y) {
		for (int x = 0; x < confidence.cols; ++x) {
			confidence[y][x] = confidence[y][x]*255.0/overall_max;
		}
	}

	// write image w/ segmentation indices
	cv::Mat_<cv::Vec3b> segmentation_index(normalized_image.rows, normalized_image.cols, cv::Vec3b(255, 255, 255));
	for (int i = 0; i < (int) segmentation.size(); i++) {
		std::vector<cv::Point> &coord= segmentation[i].coordinates;
		for (int j = 0; j < (int)coord.size(); ++j) {
			cv::Vec3b the_index;
			the_index[0] = i / (256*256);
			the_index[1] = (i % (256*256)) / 256;
			the_index[2] = i % 256;
			segmentation_index[coord[j].y][coord[j].x] = the_index;
		}
	}

	{ std::stringstream s; s << config.output_directory << "/iic_" << img_name << "_" << config.input_intensity_max << "_conf.png"; outfile = s.str(); }
	std::cout << "writing" << outfile << std::endl;
	cv::imwrite(outfile, confidence);


	{ std::stringstream s; s << config.output_directory << "/iic_" << img_name << "_" << config.input_intensity_max << "_est.png"; outfile = s.str(); }
	std::cout << "writing" << outfile << std::endl;
	cv::imwrite(outfile, color_segmented);
	{
		std::stringstream s;
		if (config.input_file.length() > 1) {
			s << config.output_directory << "/seg_idx_" << img_name << ".png";
		} else {
			s << config.output_directory << "/seg_idx_" << img_name << ".png";
		}
		outfile = s.str();
	}
	cv::imwrite(outfile, segmentation_index);
	std::cout << "wrote index file " << outfile << std::endl;

	if( config.verbosity > 0 ){
		std::cout << "statistics:" << std::endl;
		std::cout << "number of segments: " << segmentation.size() << std::endl;
	}

	return 0;
}

void CommandColorIllumField::printShortHelp() const {
	std::cout << "Local Coloring of Illuminant Estimates" << std::endl;
}

void CommandColorIllumField::printHelp() const {
	std::cout << "Local Coloring of Illuminant Estimates (modularized version)" << std::endl;
}



}
