
#include "iebv_estimator.h"

using namespace illumestimators;

namespace iic {

IebvEstimator::IebvEstimator(const ConfigIebv &config)
	: config(config)
{
	error_min_number_pixels = false;
	error_slope_out_of_bounds = false;
	error_eccentricity_too_low = false;
}

Illum IebvEstimator::estimate(const cv::Mat_<cv::Vec4d> &iic_space, const superpixels::Superpixel& superpixel)
{
	error_min_number_pixels = false;
	error_slope_out_of_bounds = false;
	error_eccentricity_too_low = false;

	std::vector<cv::Point> coords = superpixel.coordinates;

	if ((int)coords.size() < std::max<int>(1, config.min_pixels)) {
		if( config.verbosity > 1 )
			std::cout << "Not enough pixels in superpixel: superpixel.size() = " << coords.size() << ", config.min_pixels = " << config.min_pixels << std::endl;
		error_min_number_pixels = true;
		return Illum();
	}

	// compute mean of each channel for subtraction later on (R, G, B, inverse intensity)
	cv::Vec4d means(0, 0, 0, 0);
	for (int s = 0; s < (int)coords.size(); ++s) {
		for (int c = 0; c < 4; ++c) {
			means[c] += iic_space[coords[s].y][coords[s].x][c];
		}
	}

	for (int c = 0; c < 4; ++c) {
		means[c] /= coords.size();
	}

	// prepare for svd
	cv::Mat_<cv::Vec3d> iic_superpixel(2, coords.size());
	for (unsigned int idx = 0; idx < coords.size(); ++idx) {
		for (int c = 0; c < 3; ++c) {
			iic_superpixel[0][idx][c] = iic_space[coords[idx].y][coords[idx].x][3] - means[3]; // FASS DAS NICHT AN DAS IST DIE X-RICHTUNG, UND STIMMT
			iic_superpixel[1][idx][c] = iic_space[coords[idx].y][coords[idx].x][c] - means[c];
		}
	}

	std::vector<cv::Mat_<double> > channels;
	cv::split(iic_superpixel, channels);
	// each color channel has it's own row in eigenvalues and eigenvectors
	cv::Mat_<double> eigenvalues(3, 2);
	cv::Mat_<cv::Vec2d> eigenvectors(3, 2);
	double max_slope = -1;
	double min_slope = std::numeric_limits<double>::max();
	double max_eccentricity = -1;
	double min_eccentricity = std::numeric_limits<double>::max();
	for (int c = 0; c < 3; ++c) {

		channels[c] = channels[c] * channels[c].t(); // square it
		cv::SVD svd(channels[c], cv::SVD::FULL_UV);

		cv::Mat_<double> u = svd.u;

		eigenvectors[c][0][0] = u[0][0];
		eigenvectors[c][0][1] = u[1][0];
		eigenvectors[c][1][0] = u[0][1];
		eigenvectors[c][1][1] = u[1][1];

		cv::Mat_<double> w = svd.w;
		// square singular values from SVD so they become eigenvalues
		eigenvalues[c][0] = w[0][0] * w[0][0];
		eigenvalues[c][1] = w[1][0] * w[1][0];

		double slope = fabs((double)(eigenvectors[c][0][1]/eigenvectors[c][0][0]));
		if (slope > max_slope) max_slope = slope;
		if (slope < min_slope) min_slope = slope;

		double eccentricity = std::sqrt(1 - eigenvalues[c][1]/eigenvalues[c][0]);
		if (eccentricity > max_eccentricity) max_eccentricity = eccentricity;
		if (eccentricity < min_eccentricity) min_eccentricity = eccentricity;
	}

	// if all are larger than the max-limit, or all are lower than the min-limit
	if ((min_slope > config.maximum_eigenvector_slope) || (max_slope < config.minimum_eigenvector_slope)) {
		error_slope_out_of_bounds = true;
		if (config.verbosity > 1)
			std::cout << "Slope out of bounds: min_slope = " << min_slope << ", max_slope = " << max_slope << ", minimum = " << config.minimum_eigenvector_slope << ", maximum = " << config.maximum_eigenvector_slope << std::endl;
		return Illum();
	}

	// current version: if all eccentricities are smaller than the threshold
	if (max_eccentricity < config.minimum_eigenvalue_ratio) {
		error_eccentricity_too_low = true;
		return Illum();
	}

	Illum gamma(true);
	gamma.isBgr = true; // it is BGR, and it remains BGR unless we print it to STDOUT.
	for (int c = 0; c < 3; ++c) {
		gamma[c] = means[c] - (eigenvectors[c][0][1] / eigenvectors[c][0][0]) * means[3];
        if ((gamma[c] < 0) || (gamma[c] > 1)) gamma.isValid = false;
	}

//	// normalize to one
//	double sum = gamma[0] + gamma[1] + gamma[2];
//	gamma[0] /= sum;
//	gamma[1] /= sum;
//	gamma[2] /= sum;

	if ((config.verbosity > 0 ) && (gamma.isValid)){
        std::cout << "estimate:\t[" << gamma.r << ", " << gamma.g << ", " << gamma.b << "]" << std::endl;
	}

	return gamma;
}

int IebvEstimator::error() {
	int error = 0;
	error += (error_min_number_pixels ? 1 : 0);
	error += (error_slope_out_of_bounds ? 2 : 0);
	error += (error_eccentricity_too_low ? 4 : 0);
	return error;
}

}
