#ifndef ILLUMESTIMATORS_GRAYWORLD_GRAYWORLDESTIMATOR_H
#define ILLUMESTIMATORS_GRAYWORLD_GRAYWORLDESTIMATOR_H

#include "illuminantestimator.h"

namespace illumestimators {

/** The GrayWorldEstimator class provides a generalized Gray World illuminant estimator.
 *  Read J. van de Weijer, T. Gevers, and A. Gijsenij, "Edge-based color constancy,"
 *  IEEE Transactions on Image Processing, vol. 16, no. 9, pp. 2207–2214, Sept 2007.
 */
class GrayWorldEstimator : public IlluminantEstimator
{
public:
	/** Constructor that sets all paramters.
	 *  \param n Derivative order.
	 *  \param p Parameter of Minkowski norm.
	 *  \param sigma Standard deviation of Gaussian smoothing filter.
	 */
	GrayWorldEstimator(int n = 0, int p = 1, double sigma = 0);
	~GrayWorldEstimator();

public:
	std::string name() const;
	std::string identifier() const;

public:
	/** Set the derivative order.
	 *  \param n Derivative order.
	 */
	void setDerivativeOrder(int n) { m_n = n; }
	/** Set the parameter of the Minkowski norm.
	 *  \param p Parameter of Minkowski norm.
	 */
	void setMinkowskiNorm(int p) { m_p = p; }
	/** Set the standard deviation of the Gaussian smoothing filter.
	 *  \param sigma Standard deviation of Gaussian smoothing filter.
	 */
	void setSigma(double sigma) { m_sigma = sigma; }

public:
	/** Get the derivative order.
	 *  \return Derivative order.
	 */
	int getDerivativeOrder() { return m_n; }
	/** Get the parameter of the Minkowski norm.
	 *  \return Parameter of Minkowski norm.
	 */
	int getMinkowskiNorm() { return m_p; }
	/** Get the standard deviation of the Gaussian smoothing filter.
	 *  \return Standard deviation of Gaussian smoothing filter.
	 */
	double getSigma() { return m_sigma; }

public:
	using IlluminantEstimator::estimateIlluminant;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const;
	Illum estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel &superpixel, const cv::Mat_<unsigned char>& mask) const;

public:
	bool train(const std::vector<std::string>& imageFiles, const std::vector<std::string>& colorspaces, const std::vector<cv::Vec3d>& illuminants, const std::vector<std::string>& maskFiles);

public:
	bool save(const std::string& filename) const;
	bool load(const std::string& filename);
	virtual int error();

	/** Run preprocessing on the input image data.
	 *  \param image Input image.
	 *  \param mask Input mask.
	 */
	void preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const;

private:
	/** Calculate the Minkowski norm.
	 *  \param pixels Image vector.
	 *  \param p Parameter of Minkowski norm.
	 */
	Illum minkowskiNorm(const std::vector<cv::Vec3d>& pixels, int p) const;

private:
	/** Derivative order. */
	int m_n;
	/** Parameter of Minkowski norm. */
	int m_p;
	/** Standard deviation of Gaussian smoothing filter. */
        double m_sigma;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_GRAYWORLD_GRAYWORLDESTIMATOR_H
