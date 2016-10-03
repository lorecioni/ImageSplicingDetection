#include "single_illuminant_pipeline.h"
#include "common/color.h"
#include "metadata/metadatastorage.h"
#include "evaluation/error.h"
#include <iostream>
#include <time.h>
#include <fstream>
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3
#endif // BOOST_FILESYSTEM_VERSION
#include <boost/filesystem.hpp>

namespace illumestimators {

bool SingleIlluminantPipeline::loadIlluminantEstimator(IlluminantEstimator& estimator, const std::string& filename)
{
	if (filename.size() > 0) {
		if (estimator.load(filename)) {
			std::cout << "Loading illuminant estimator from file." << std::endl << std::endl;
			return true;
		} else {
			std::cout << "Failed loading illuminant estimator from file." << std::endl << std::endl;
		}
	}

	return false;
}

bool SingleIlluminantPipeline::saveIlluminantEstimator(const IlluminantEstimator& estimator, const std::string& filename)
{
	if (filename.size() > 0) {
		estimator.save(filename);
		return true;
	}

	return false;
}

bool SingleIlluminantPipeline::runTraining(IlluminantEstimator& estimator, const std::vector<std::string>& filenames, MetadataStorage& storage, int verbosity)
{
	std::vector<std::string> trainingFilenames;

	for (unsigned int i = 0; i < filenames.size(); i++) {
		std::string filename = filenames[i];
		boost::filesystem::path path(filename);

		if (path.extension().string().compare(".txt") == 0) {
			std::ifstream in(filename.c_str(), std::ifstream::in);

			while (in.good()) {
				std::string line;
				std::getline(in, line);

				if (line.size() > 0) {
					if (boost::filesystem::path(line).is_absolute()) {
						trainingFilenames.push_back(line);
					} else {
						trainingFilenames.push_back(path.parent_path().string() + "/" + line);
					}
				}
			}
		} else {
			trainingFilenames.push_back(filename);
		}
	}

	if (verbosity > 0) {
		std::cout << "Training " << estimator.name();
		if (verbosity > 1) {
			std::cout << " on the following training files:" << std::endl;
			for (unsigned int i = 0; i < trainingFilenames.size(); i++) {
				 std::cout << "  " << trainingFilenames[i] << std::endl;
			}
		} else {
			std::cout << std::endl;
		}
	}

	std::vector<std::string> trainingColorspaces;
	std::vector<cv::Vec3d> trainingIlluminants;
	std::vector<std::string> trainingMasks;
	std::vector<std::string> trainingFiles;

	for (unsigned int i = 0; i < trainingFilenames.size(); i++) {
		Metadata meta = storage.getMetadata(trainingFilenames[i]);

		trainingFiles.push_back(meta.filename());
		trainingColorspaces.push_back(meta.colorspace());
		trainingIlluminants.push_back(cv::Vec3d(meta.dominantIlluminant().b, meta.dominantIlluminant().g, meta.dominantIlluminant().r));
		trainingMasks.push_back(meta.maskFilename());
	}

	if (trainingIlluminants.size() == 0) {
		std::cerr << "Error no training data." << std::endl << std::endl;
		return false;
	}

	clock_t start = clock();

	estimator.train(trainingFiles, trainingColorspaces, trainingIlluminants, trainingMasks);

	clock_t elapsed = (clock() - start) / 1000;

	if (verbosity > 0) {
		std::cout << "Training: " << elapsed << "ms" << std::endl << std::endl;
	}

	return true;
}

bool SingleIlluminantPipeline::runEstimator(Illum& estimate, double& error, const IlluminantEstimator& estimator, const std::string& filename, MetadataStorage& storage, int verbosity)
{
	if (!storage.hasMetadata(filename)) {
		std::cerr << "Error loading meta data file for " << filename << ", aborted." << std::endl;
		return false;
	}

	Metadata meta = storage.getMetadata(filename);

	cv::Mat_<cv::Vec3d> inputImage = IlluminantEstimator::cleanlyReadImage(filename, meta.colorspace());

	if (inputImage.empty()) {
		std::cerr << "Error loading input image " << filename << "!" << std::endl;
		return false;
	}

	cv::Mat_<unsigned char> mask = meta.mask();

	if (mask.empty()) {
		mask = cv::Mat_<unsigned char>(inputImage.rows, inputImage.cols, (unsigned char)0);
	}

	if (verbosity > 0) {
		std::cout << "Calling " << estimator.name() << ":" << std::endl;
		std::cout << "  " << filename << std::endl << std::endl;
	}

	cv::Mat_<cv::Vec3d> outputImage;
	cv::Mat_<unsigned char> outputMask;

	clock_t start = clock();

	estimator.preprocessImage(inputImage, mask, outputImage, outputMask);
	estimate = estimator.estimateIlluminant(outputImage, outputMask);

	clock_t elapsed = (clock() - start) / 1000;

	if (verbosity > 0) {
		std::cout << "Estimation: " << elapsed << "ms" << std::endl << std::endl;
	}

	Illum groundtruth = meta.dominantIlluminant();

	if (verbosity > 0) {
		std::cout << "Ground truth illuminant chromaticities:" << std::endl;
		std::cout << "  r = " << groundtruth.r << ", g = " << groundtruth.g << ", b = " << groundtruth.b << std::endl << std::endl;

		std::cout << "Estimated illuminant chromaticities:" << std::endl;
		std::cout << "  r = " << estimate.r << ", g = " << estimate.g << ", b = " << estimate.b << std::endl << std::endl;

		error = estimate.angularErrorDeg(groundtruth);
		std::cout << "Error: " << error << "°" <<  std::endl << std::endl;
	} else {
		error = estimate.angularErrorDeg(groundtruth);
		std::cout << error <<  std::endl;
	}
	return true;
}

} // illumestimators
