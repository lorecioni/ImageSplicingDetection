#include "command_lgrayworld.h"
#include "grayworld/grayworldestimator.h"
#include "img_read.h"
#include "superpixelsegmentation.h"
#include "paint_superpixel.h"

#include <opencv2/highgui/highgui.hpp>

namespace lille {

CommandLocalGrayworld::CommandLocalGrayworld():
	Command("lgrayworld",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de"
	)
{
}

int CommandLocalGrayworld::execute()
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

	if (config.segmentationFile.length() < 1) {
		std::cerr << "segmentation file name required!" << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec3b> segImage = cv::imread(config.segmentationFile);
	if ((segImage.rows != inputImage.rows) || (segImage.cols != inputImage.cols)) {
		std::cerr  << "segmentation image dimension (" << segImage.rows << "x" << segImage.cols
			<< ") does not match image dimension (" << inputImage.rows << "x" << inputImage.cols << "), aborted." << std::endl;
		return 1;
	}
	std::vector<superpixels::Superpixel> superpixels;
	superpixels::SuperpixelSegmentation::imageToSuperpixels(segImage, superpixels);

	illumestimators::GrayWorldEstimator est(config.n, config.p, config.sigma);
	cv::Mat_<cv::Vec3d> image;
	cv::Mat_<unsigned char> mask;
	est.preprocessImage(inputImage, raw_mask, image, mask);

	cv::Mat_<cv::Vec3b> estimated_illuminants(image.rows, image.cols, cv::Vec3b(0, 0, 0));

	// for every superpixel:
	// 	collect pixels within the mask
	// 	if # pixels too small: black
	// 	else: estimate on these pixels
	// 	paint color in output image
	// 	write output image
	for (unsigned int s = 0; s < superpixels.size(); ++s) {
		illumestimators::Illum illum = est.estimateIlluminant(image, superpixels[s], mask);
		PaintSuperpixel::paint(estimated_illuminants, superpixels[s], illum);
	}

	cv::imwrite(config.outputFile, estimated_illuminants);

/*
	illumestimators::GrayWorldEstimator estimator(config.n, config.p, config.sigma);

	SingleIlluminantPipeline::loadIlluminantEstimator(estimator, config.loadFile);

	MetadataStorage storage(config.metaFiles);

	Illum estimate;
	double error;

	if (!SingleIlluminantPipeline::runEstimator(estimate, error, estimator, config.inputFile, storage, config.verbosity)) {
		return -1;
	}

	SingleIlluminantPipeline::saveIlluminantEstimator(estimator, config.saveFile);
*/

	return 0;
}

void CommandLocalGrayworld::printShortHelp() const
{
	std::cout << "Locally applied illuminant estimator, using the generalized Gray World algorithm." << std::endl;
}

void CommandLocalGrayworld::printHelp() const
{
	std::cout << "Locally applied illuminant estimator, using the generalized Gray World algorithm." << std::endl;
}

} // namespace illumestimators
