#ifndef IIC_IEBV_H
#define IIC_IEBV_H

#include <opencv2/imgproc/imgproc.hpp>

#include "illuminantestimator.h"
#include "config_iebv.h"
#include "iebv_estimator.h"
#include "superpixel.h"

namespace iic {

/** Our iic-based estimator for the illuminant color
 */
class Iebv : public illumestimators::IlluminantEstimator {
public:

	Iebv(const ConfigIebv &config);
	Iebv(const ConfigIebv &config, cv::Mat_<cv::Vec4d> iic_space);

	/** Estimate the dominant illuminant of an image.
	 *  \param image Input image in BGR format.
	 *  \return Illuminant chromaticities vector in BGR format.
	 */
	illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image) const;
	/** Estimate the dominant illuminant of an image.
	 *  \param image Input image in BGR format.
	 *  \param mask Image mask.
	 *  \return Illuminant chromaticities vector in BGR format.
	 */
	virtual illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const;
	/** Estimate the dominant illuminant of a superpixel of an image.
	 *  \param image Input image in BGR format.
	 *  \param superpixel Image coordinates of the superpixel.
	 *  \param mask Image mask.
	 *  \return Illuminant chromaticities vector in BGR format.
	 */
	illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel) const;
	/** Estimate the dominant illuminant of a superpixel of an image.
	 *  \param image Input image in BGR format.
	 *  \param superpixel Image coordinates of the superpixel.
	 *  \param mask Image mask.
	 *  \return Illuminant chromaticities vector in BGR format.
	 */
	virtual illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const;

	// FIXME NOT IMPLEMENTED!
	virtual illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec<double, 3> >&, const std::vector<cv::Vec<int, 2>, std::allocator<cv::Vec<int, 2> > >&, const cv::Mat_<unsigned char>&) const { std::cout << "OOPS NOT IMPLEMENTED"; return illumestimators::Illum(); }


public:
	virtual void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;
	virtual bool save(const std::string&) const;
	virtual bool load(const std::string&);

	/** new image? force recomputation of iic space */
	void createIICSpace(cv::Mat_<cv::Vec3d> &image);
	virtual int error();

public:
	/** Train illuminant estimator with labelled images.
	 *  \param imageFiles Filenames of training images.
	 *  \param colorspaces Colorspaces of training images.
	 *  \param illuminants Illuminant chromaticities vectors of training images.
	 *  \param maskFiles Filenames of mask images.
	 */
	virtual bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles = std::vector<std::string>());

public:
	/** Name of illuminant estimator with current parameters for display purposes.
	 *  \return Name of illuminant estimator.
	 */
	virtual std::string name() const;
	/** Unique identifier of illuminant estimator given current parameters.
	 *  \return Unique identifier of illuminant estimator.
	 */
	virtual std::string identifier() const;

protected:
	const ConfigIebv &config;
	IebvEstimator estimator;
	cv::Mat_<cv::Vec4d> iic_space;
	bool iic_space_set;


	/** randomly selects a patch region within the given image
	 *  \param image Input image in BGR format.
	 * \param width defines the width of the patch
	 * \param height defines the height of the patch
	 * \param mask an optional mask in which area not more than 50% of the patch's pixels shall lie
	 * \return a vector of OpenCV Point structures, defining the patch
	 */
	superpixels::Superpixel selectPatchRandomly(const cv::Mat_<cv::Vec3d>& image, int width, int height, const cv::Mat_<unsigned char>* mask = 0, const cv::Mat_<unsigned char>* excluded_area = 0) const;
};

}

#endif // IIC_IEBV_H
