#include "command_ltan.h"
#include "img_read.h"
#include "superpixelsegmentation.h"
#include "paint_superpixel.h"
#include "illum.h"
#include "tan_orthodox/orthodox_tan_adapter.h"

#include <opencv2/highgui/highgui.hpp>

namespace lille {

CommandLocalTan::CommandLocalTan():
	Command("ltan",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de"
	)
{
}

// what does the command expect?
// - a configuration file for IEBV
// - an unknown input image.
// - segmentation of the image
// - a mask for the image
int CommandLocalTan::execute()
{

	if (config.img.imageFile.length() < 1) {
		std::cerr << "input file name required!" << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec3d> inputImage = iread::IRead::img_read_dl(
		config.img.imageFile,
		config.img.color_space,
		config.img.dark_level,
		config.img.saturation_level
	);
	if (inputImage.rows == 0) {
		std::cerr << "unable to read input image " << config.img.imageFile << ", aborted." << std::endl;
		return 1;
	}

	if (config.segmentationFile.length() < 1) {
		std::cerr << "segmentation image file (-S) required!" << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec3b> segImage = cv::imread(config.segmentationFile);
	if ((segImage.rows != inputImage.rows) || (segImage.cols != inputImage.cols)) {
		std::cerr  << "segmentation image dimension (" << segImage.rows << "x" << segImage.cols
			<< ") does not match image dimension (" << inputImage.rows << "x" << inputImage.cols << "), aborted." << std::endl;
		return 1;
	}

	if (config.outputFile.length() < 1) {
		std::cerr << "output image name (-O) required!" << std::endl;
		return 1;
	}
	cv::Mat_<unsigned char> raw_mask;
	if (config.maskFile.length() > 0) {
		raw_mask = cv::imread(config.maskFile, 0);
		if ((raw_mask.rows != inputImage.rows) || (raw_mask.cols != inputImage.cols)) {
			std::cerr << "mask dimension (" << raw_mask.rows << "x" << raw_mask.cols
				<< ") does not match image dimension (" << inputImage.rows << "x" << inputImage.cols << "), aborted." << std::endl;
			return 1;
		}
	} else {
		raw_mask = cv::Mat_<unsigned char>(inputImage.rows, inputImage.cols, (unsigned char)0);
	}

	std::vector<superpixels::Superpixel> superpixels;
	superpixels::SuperpixelSegmentation::imageToSuperpixels(segImage, superpixels);

	OrthodoxTanAdapter tan;

	cv::Mat_<cv::Vec3b> estimated_illuminants(segImage.rows, segImage.cols, cv::Vec3b(0, 0, 0));

	// for every superpixel:
	// 	collect pixels within the mask
	// 	if # pixels too small: black
	// 	else: estimate on these pixels
	// 	paint color in output image
	// 	write output image
	for (unsigned int s = 0; s < superpixels.size(); ++s) {
		// for every superpixel, create a pseudo-image with the colors of the superpixel
		if (superpixels[s].coordinates.size() < 3) continue;
		int minX = 10000; int maxX = -1;
		int minY = 10000; int maxY = -1;
		int nPixels = 0;
		for (int i = 0; i < (int)superpixels[s].coordinates.size(); ++i) {
			cv::Point &p = superpixels[s].coordinates[i];
			if (raw_mask[p.y][p.x] == 255) continue;
			nPixels++;
			if (p.x > maxX) maxX = p.x; if (p.x < minX) minX = p.x;
			if (p.y > maxY) maxY = p.y; if (p.y < minY) minY = p.y;
		}
		if (nPixels < 3) continue;
		cv::Mat_<cv::Vec3b> sp_img(maxY-minY+1, maxX-minX+1, cv::Vec3b(0, 0, 0));
		for (int i = 0; i < (int)superpixels[s].coordinates.size(); ++i) {
			cv::Point &p = superpixels[s].coordinates[i];
			if (raw_mask[p.y][p.x] == 255) continue;
			for (int c = 0; c < 3; ++c) 
				sp_img[p.y-minY][p.x-minX][c] = (unsigned char)(inputImage[p.y][p.x][c]*255);
		}
		illumestimators::Illum illum = tan.estimate(sp_img);

		PaintSuperpixel::paint(estimated_illuminants, superpixels[s], illum);
	}

	cv::imwrite(config.outputFile, estimated_illuminants);

	return 0;
}

void CommandLocalTan::printShortHelp() const
{
	std::cout << "Locally applied illuminant estimator, using IEBV." << std::endl;
}

void CommandLocalTan::printHelp() const
{
	std::cout << "Locally applied illuminant estimator, using IEBV." << std::endl;
}

} // namespace illumestimators
