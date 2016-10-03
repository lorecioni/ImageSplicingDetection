#include "command_gw_orig.h"
#include "grayworld/grayworldestimator.h"
#include "img_read.h"

#include <opencv2/highgui/highgui.hpp>

namespace lille {

CommandGrayworldOrig::CommandGrayworldOrig():
	Command("gwOrig",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de"
	)
{
}

int CommandGrayworldOrig::execute()
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

	illumestimators::GrayWorldEstimator est(config.n, config.p, config.sigma);
	cv::Mat_<cv::Vec3d> image;
	cv::Mat_<unsigned char> mask;
	est.preprocessImage(inputImage, raw_mask, image, mask);


	illumestimators::Illum illum = est.estimateIlluminant(image, mask);

	cv::Vec3b the_estimate(illum.b*255, illum.g*255, illum.r*255);
	cv::Mat_<cv::Vec3b> estimated_illuminants(image.rows, image.cols, the_estimate);

	cv::imwrite(config.outputFile, estimated_illuminants);

	return 0;
}

void CommandGrayworldOrig::printShortHelp() const
{
	std::cout << "Perform original (orthodox) Grayworld on a single image (no superpixels)." << std::endl;
}

void CommandGrayworldOrig::printHelp() const
{
	std::cout << "Perform original (orthodox) Grayworld on a single image (no superpixels)." << std::endl;
}

} // namespace illumestimators
