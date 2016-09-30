#ifndef IIC_TAN_H
#define IIC_TAN_H

#include "illuminantestimator.h"
#include "config_iebv.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace iic {

/** Robby Tan's estimator for the illuminant color
 */
class Tan : public illumestimators::IlluminantEstimator {
public:

	Tan(const ConfigIebv &config);

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
	illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel) const;
	/** Estimate the dominant illuminant of a superpixel of an image.
	 *  \param image Input image in BGR format.
	 *  \param superpixel Image coordinates of the superpixel.
	 *  \param mask Image mask.
	 *  \return Illuminant chromaticities vector in BGR format.
	 */
	virtual illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const;

	// FIXME NOT IMPLEMENTED!
	virtual illumestimators::Illum estimateIlluminant(const cv::Mat_<cv::Vec<double, 3> >&, const std::vector<cv::Vec<int, 2>, std::allocator<cv::Vec<int, 2> > >&, const cv::Mat_<unsigned char>&) const {std::cout << "OOPS NOT IMPLEMENTED"; return illumestimators::Illum(); }


public:
	virtual void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;
	virtual bool save(const std::string&) const;
	virtual bool load(const std::string&);
	virtual int error(); // dummy implementation, returns always 0


	/** Train illuminant estimator with labelled images.
	 *  \param imageFiles Filenames of training images.
	 *  \param colorspaces Colorspaces of training images.
	 *  \param illuminants Illuminant chromaticities vectors of training images.
	 *  \param maskFiles Filenames of mask images.
	 */
	virtual bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles = std::vector<std::string>());

public:
	void createIICSpace(cv::Mat_<cv::Vec3d> &image);
	/** Name of illuminant estimator with current parameters for display purposes.
	 *  \return Name of illuminant estimator.
	 */
	virtual std::string name() const;
	/** Unique identifier of illuminant estimator given current parameters.
	 *  \return Unique identifier of illuminant estimator.
	 */
	virtual std::string identifier() const;

private:
	ConfigIebv config;
	bool iic_space_set;
	cv::Mat_<cv::Vec4d> iic_space;


};

}

#endif // IIC_IIC_TAN_H
