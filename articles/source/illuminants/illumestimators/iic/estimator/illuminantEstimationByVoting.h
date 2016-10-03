#ifndef ILLUMINANT_ESTIMATION_BY_VOTING_H
#define ILLUMINANT_ESTIMATION_BY_VOTING_H

// TODO: remove this whole file, shouldn't be necessary anymore

#include "iebv_config.h"
#include "iebv_result_images.h"
#include "patch.h"
#include "pixel_selection_strategy.h"

#include "cv.h"

/** Engine for the Illuminant Estimation by Voting algorithm. Command line
 * invocation can be done by calling iebv.
 */
class illuminantEstimationByVoting {

public:
	// TODO CLEANUP (see also iebv.h/.cxx)
	unsigned int patches_drawn;
	unsigned int patches_good;

	illuminantEstimationByVoting(IebvConfig *cfg, iebvResultImages *result_images);

	cv::Vec3d fully_automated(PixelSelectionStrategy *strat);

	bool adhocChecks(
        std::vector<cv::Vec3d > &pixels,
        std::vector<cv::Vec3d > &pixels_chrom,
		std::vector<double> &intensitySum,
        unsigned int                   patch_number
	);

	bool tanEstimate(
        unsigned int           patch_number,
		std::vector<double>    &intensitySum,
		std::vector<cv::Vec3d> &pixels_chrom,
        cv::Vec3d              &gamma
	);

	bool eigenvalueChecks(
        unsigned int           patch_number,
		std::vector<double>    &intensitySum,
		std::vector<cv::Vec3d> &pixels_chrom,
        cv::Vec3d              &gamma
	);

	double getConfidence(
		std::vector<cv::Vec3d > &pixels,
        unsigned int patch_number,
		cv::Vec3d &slope, cv::Vec3d &ecc,
		std::vector<std::vector<double> > &means,
		std::vector<std::vector<double> > &eigenvals, std::vector<std::vector<double> > &eigenvecs,
		cv::Vec3d &gamma
	);

	bool getPatchEstimate(
        std::vector<cv::Vec3d> &pixels,
        unsigned int           patch_number,
        cv::Vec3d              &gamma
	);

	static void reportPatchQuality(
		unsigned int patch_id,
		unsigned int verbosity,
		bool edges_ok = true,
		bool minimum_number_pixels_ok = true,
		bool chromaticity_std_dev_ok = true,
		bool intensity_std_dev_ok = true,
		bool inverse_intensity_range_ok = true,
		bool eigenvector_slope_ok = true,
		bool eccentricity_ok = true
	);

	IebvConfig *cfg;

protected:
//	void printPatchStatus(Patch<unsigned char> &p, int patch_number);

	cv::Mat_<unsigned char> *img;
	iebvResultImages *result_images;
};

#endif // ILLUMINANT_ESTIMATION_BY_VOTING_H
