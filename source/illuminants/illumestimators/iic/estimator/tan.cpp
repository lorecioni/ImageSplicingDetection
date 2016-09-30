#include "tan.h"
#include "iic.h"
#include "common/mask.h"

using namespace illumestimators;

namespace iic {

/** Robby Tan's estimator for the illuminant color
 */
Tan::Tan(const ConfigIebv &config)
	: config(config), iic_space_set(false)
{
}

/**
 * Repackage data in the inverse intensity chromaticity space. The first three
 * channels contain red, green and blue chromaticities. The fourth channel
 * contains 1/(r+g+b).
 * Note that it is necessary to call this method prior to the estimation.
 */
void Tan::createIICSpace(cv::Mat_<cv::Vec3d> &image)
{
//	IIC iic((cv::Mat_<cv::Vec3d>)image, config.min_intensity, config.max_intensity);
	IIC iic((cv::Mat_<cv::Vec3d>)image);
	iic_space = iic.getIIC();
	iic_space_set = true;
}


/** Estimate the dominant illuminant of an image using Lehmann/Palm specularity segmentation.
 *  \param image Input image in BGR format.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Tan::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image) const
{
	return Illum();
}

/** Estimate the dominant illuminant of an image using a specularity mask.
 *  \param image Input image in BGR format.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Tan::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const
{
	return Illum();
}

/** Estimate the dominant illuminant of a superpixel of an image using Lehmann/Palm
 *  \param image Input image in BGR format.
 *  \param superpixel Image coordinates of the superpixel.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Tan::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel) const
{
	if (!iic_space_set) {
		if (config.verbosity > 0) std::cerr << "Tan::estimateIlluminant: iic space not set, aborted." << std::endl;
		return Illum();
	}
	Illum gamma(true);
	if (config.verbosity > 1) std::cout << "Tan:" << std::endl;
	int nPixels = (int)superpixel.size();
	int accu[100];
	// color channels
	for (int c = 0; c < 3; c++) {
		for (int i = 0; i < 100; ++i) accu[i] = 0;
		for (int i = 0; i < nPixels; ++i) {
			for (int j = i+1; j < nPixels; ++j) {
				if (iic_space[superpixel[i].y][superpixel[i].x][3] == iic_space[superpixel[j].y][superpixel[j].x][3]) continue; // catch division by zero;

				// compute line between two pixels and the intersection with the y-axis
				double x1 = 1.0 / iic_space[superpixel[i].y][superpixel[i].x][3];
				double y1 = iic_space[superpixel[i].y][superpixel[i].x][c];
				double x2 = 1.0 / iic_space[superpixel[j].y][superpixel[j].x][3];
				double y2 = iic_space[superpixel[j].y][superpixel[j].x][c];
				
				double slope = (y2-y1)/(x2-x1);
				double estimate = slope * (-x1) + y1;
				if (isnan(estimate) || (estimate == std::numeric_limits<double>::infinity())) continue;
				int accupos = estimate * 100;
				if ((accupos <= 0) || (accupos >= 100)) continue;
				accu[accupos]++;
			}
		}
		// smooth the accumulator (currently, only kind-of-averaging is applied)
		int accu2[100];
		accu2[0] = 0; accu2[1] = 0; accu2[98] = 0; accu2[99] = 0;
		for (int i = 2; i < 98; ++i) accu2[i] = accu[i-2] + 2*accu[i-1] + 4*accu[i] + 2*accu[i+1] + accu[i+2];
		int max = -1; int maxPos = -1;
		for (int i = 1; i < 99; ++i) {
			if (accu2[i] > max) {
				max = accu2[i];
				maxPos = i;
			}
		}
		gamma[c] = static_cast<double>(maxPos) / 100.0;
	}
	// normalize to 1
	double sum = gamma[0] + gamma[1] + gamma[2];
	for (int i = 0; i < 3; ++i) gamma[i] = gamma[i] / sum;
	return gamma;
}

/** Estimate the dominant illuminant of a superpixel of an image.
 *  \param image Input image in BGR format.
 *  \param superpixel Image coordinates of the superpixel.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Tan::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	return Illum();
}

void Tan::preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const
{
	outputImage = image.clone();
	if ((image.rows != mask.rows) || (image.cols != mask.cols)) {
		std::cerr << "No mask!" << std::endl;
		outputMask = cv::Mat_<unsigned char>::zeros(outputImage.rows, outputImage.cols);
	} else {
		outputMask = mask.clone();
	}
	illumestimators::Mask::maskSaturatedPixels(outputImage, outputMask, config.max_intensity);
	illumestimators::Mask::maskDarkPixels(outputImage, outputMask, config.min_intensity);
}

/* no machine learning involved - no saving of training data */
bool Tan::save(const std::string&) const
{
	return true;
}

/* no machine learning involved - no loading of training data */
bool Tan::load(const std::string&)
{
	return true;
}

int Tan::error()
{
	return 0;
}



/** Train illuminant estimator with labelled images.
 *  \param imageFiles Filenames of training images.
 *  \param colorspaces Colorspaces of training images.
 *  \param illuminants Illuminant chromaticities vectors of training images.
 *  \param maskFiles Filenames of mask images.
 */
bool Tan::train(
	const std::vector<std::string>& imageFiles,
	const std::vector<std::string>& colorspaces,
	const std::vector<cv::Vec3d>& illuminants,
	const std::vector<std::string>& maskFiles)
{
	return true;
}

/** Name of illuminant estimator with current parameters for display purposes.
 *  \return Name of illuminant estimator.
 */
std::string Tan::name() const
{
	return std::string("Tan/Nishino/Ikeuchi IIC Estimator");
}

/** Unique identifier of illuminant estimator given current parameters.
 *  \return Unique identifier of illuminant estimator.
 */
std::string Tan::identifier() const
{
	return std::string("tan");
}



}


