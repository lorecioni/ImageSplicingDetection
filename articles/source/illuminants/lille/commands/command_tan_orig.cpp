#include "command_tan_orig.h"
#include "img_read.h"
#include "superpixelsegmentation.h"
#include "paint_superpixel.h"
#include "illum.h"
#include "tan_orthodox/orthodox_tan_adapter.h"

#include <opencv2/highgui/highgui.hpp>

namespace lille {

CommandTanOrig::CommandTanOrig():
	Command("tanOrig",
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
int CommandTanOrig::execute()
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

	OrthodoxTanAdapter tan;

	cv::Mat_<cv::Vec3b> sp_img(inputImage.rows, inputImage.cols, cv::Vec3b(0, 0, 0));
	for (int y = 0; y < inputImage.rows; ++y) {
		for (int x = 0; x < inputImage.cols; ++x) {
			for (int c = 0; c < 3; ++c) 
				sp_img[y][x][c] = inputImage[y][x][c] * 255;
		}
	}
	illumestimators::Illum illum = tan.estimate(sp_img);
	double corr = illum.r + illum.b + illum.g;
	if (corr > std::numeric_limits<double>::epsilon()) {
		corr = 255 / corr;
	} else {
		corr = 1;
	}
		
	cv::Mat_<cv::Vec3b> estimate(inputImage.rows, inputImage.cols, cv::Vec3b(illum.b*corr, illum.g*corr, illum.r*corr));

	cv::imwrite(config.outputFile, estimate);

	return 0;
}

void CommandTanOrig::printShortHelp() const
{
	std::cout << "Perform original (orthodox) Tan on a single image (no superpixels)." << std::endl;
}

void CommandTanOrig::printHelp() const
{
	std::cout << "Perform original (orthodox) Tan on a single image (no superpixels)." << std::endl;
}

} // namespace illumestimators
