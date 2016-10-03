// openCV

// TODO: remove this whole file, shouldn't be necessary anymore

#include "cv.h"
#include "highgui.h"

#include <iostream>
#include <limits>

#include "pixel_operations.h"
#include "illuminantEstimationByVoting.h"
#include "math_operations.h"
#include "global_defines.h"
#include "pixel_selection_strategy.h"

#ifndef IEBV_REJECTED
#define IEBV_REJECTED false
#endif

#ifndef IEBV_ACCEPTED
#define IEBV_ACCEPTED true
#endif

#ifdef WIN32 // visual studio fuckup
#define isnan(a) (a != a)
#endif

illuminantEstimationByVoting::illuminantEstimationByVoting(IebvConfig *cfg, iebvResultImages *result_images)
	: cfg(cfg), result_images(result_images)
{ 
}

cv::Vec3d illuminantEstimationByVoting::fully_automated(PixelSelectionStrategy *strat)
{
	cfg->setRandomSeed();

	// main loop
	int number_tries_left = cfg->number_tries;
	int number_useful_patches = 0;

	patches_good = 0;
	while ((number_useful_patches < cfg->number_patches) && (number_tries_left--)) {
		unsigned int patch_no = static_cast<unsigned int>(cfg->number_tries - number_tries_left);
		std::vector<cv::Vec3d> pixels;

		// abandon spatial information, wrap all entries in a vector, leaving
		// out saturated or too dark pixels.
		// includes the selected filtering steps (felzenszwalb/huttenlocher
		// segmentation, ciurea/funt mask, high/low intensities)
		if (!strat->select(patch_no, pixels)) continue; // patch early rejected?

		cv::Vec3d gamma;
		// TODO CLEANUP - what exactly?!
		if (getPatchEstimate(pixels, patch_no, gamma) == true)
		{
			patches_good++;
			// register this gamma estimate in the voting diagram
			result_images->registerVote(gamma[0], gamma[1], gamma[2]);
			number_useful_patches++;

			if (!cfg->no_plots) result_images->annotatePatch(strat->getCurrentPatch(), IEBV_ACCEPTED);

			if (cfg->verbosity > 0)
				std::cout << "Patch " << patch_no << " estimate:\t[" << gamma[0] <<
					", " << gamma[1] << ", " << gamma[2] << "]" << std::endl;
		} else {
			if (!cfg->no_plots) result_images->annotatePatch(strat->getCurrentPatch(), IEBV_REJECTED);
			if (cfg->verbosity > 1) std::cout << "rejected" << std::endl;
			continue;
		}
	}

	patches_drawn = cfg->number_tries - number_tries_left;

	// do the final voting
	result_images->smoothVotes(1);
	//result_images->smoothVotes(11);
	if (!cfg->no_plots) result_images->saveVoting(cfg->output_directory);

	// obtain illuminant estimate
	cv::Vec3d estimate;
	result_images->getVotingResult(estimate[0], estimate[1], estimate[2]);
	double sum = estimate[0]+estimate[1]+estimate[2];
	if (fabs((double)sum) < std::numeric_limits<double>::epsilon()) sum = 1;
	estimate[0] /= sum; estimate[1] /= sum; estimate[2] /= sum;

	if (!cfg->no_plots) {
		result_images->saveIid(cfg->output_directory);
		// save selected patches
		result_images->saveAnnotated(cfg->output_directory);
	}
	return estimate;
}

bool illuminantEstimationByVoting::adhocChecks(
        std::vector<cv::Vec3d> &pixels,
        std::vector<cv::Vec3d> &pixels_chrom,
		std::vector<double> &intensitySum,
        unsigned int        patch_number)
{
	// std-deviation of intensities and chromaticities must be within a threshold
	double stdDevR, stdDevG, stdDevB, stdDevRChrom, stdDevGChrom, stdDevBChrom;
	PixelOperations::stdDevPerChannel<double>(pixels, stdDevR, stdDevG, stdDevB);
	PixelOperations::stdDevPerChannel<double>(pixels_chrom, stdDevRChrom, stdDevGChrom, stdDevBChrom);

	if (
		(stdDevRChrom >= cfg->max_std_dev_chroma) ||
		(stdDevGChrom >= cfg->max_std_dev_chroma) ||
		(stdDevBChrom >= cfg->max_std_dev_chroma))
	{
		if (cfg->verbosity > 1) std::cout << "ups, stdDevRChrom = " << stdDevRChrom << ", stdDevGChrom = " << stdDevGChrom << ", stdDevBChrom = " << stdDevBChrom << "\t(allowed: " << cfg->max_std_dev_chroma << ")" << std::endl;
		reportPatchQuality(patch_number, cfg->verbosity, true, true, false);
		return false;
	}

	if ((stdDevR + stdDevG + stdDevB) < cfg->min_sum_std_dev_intensities) {
		if (cfg->verbosity > 1)
			std::cout << "operating on " << pixels.size() << " pixels, stdDevR = " << stdDevR << ", stdDevG = " << stdDevG << ", stdDevB = " << stdDevB << std::endl;
		reportPatchQuality(patch_number, cfg->verbosity, true, true, true, false);
		return false;
	}

	double min, max;
	PixelOperations::getMinMax(intensitySum, min, max);
	double range = (1.0 / min) - (1.0 / max);
	if (range < cfg->min_inverse_intensity_range) {
		reportPatchQuality(patch_number,
			cfg->verbosity, true, true, true, true,
			false);
		if (cfg->verbosity > 0) std::cout << "min_inverse_intensity_range = " << cfg->min_inverse_intensity_range << ", but we have " << range << " (on " << pixels.size() << " pixels)" << std::endl;
	}
	return true;
}


bool illuminantEstimationByVoting::eigenvalueChecks(
        unsigned int           patch_number,
		std::vector<double>    &intensitySum,
		std::vector<cv::Vec3d> &pixels_chrom,
        cv::Vec3d              &gamma
	)
{
	// red, green and blue diagram (chroma - inverse intensity)
	cv::Mat_<double> diagR, diagG, diagB;
	// note that channels are not in openCV order here
	MathOperations::intSumAndChroma2Mat<double>(intensitySum, pixels_chrom, 0, diagR);
	MathOperations::intSumAndChroma2Mat<double>(intensitySum, pixels_chrom, 1, diagG);
	MathOperations::intSumAndChroma2Mat<double>(intensitySum, pixels_chrom, 2, diagB);

	cv::Mat_<double> eigenvectorsR, eigenvectorsG, eigenvectorsB;
	cv::Mat_<double> eigenvaluesR, eigenvaluesG, eigenvaluesB;
	cv::Mat_<double> meansR, meansG, meansB;

	// means are important for later...
	MathOperations::colMeans<double>(diagR, meansR);
	MathOperations::colMeans<double>(diagG, meansG);
	MathOperations::colMeans<double>(diagB, meansB);

	MathOperations::customPCA(diagR, eigenvectorsR, eigenvaluesR);
	MathOperations::customPCA(diagG, eigenvectorsG, eigenvaluesG);
	MathOperations::customPCA(diagB, eigenvectorsB, eigenvaluesB);

	// check eigenvector slope
	// FIXME bug in matlab code? 
	// EDIT: maybe the eigenvalues/eigenvectors are in different order in matlab
	//double slopeR = std::abs<double>(eigenvectorsR[0]/eigenvectorsR[1]);
	double slopeR = GlobalDefines::abs<double>(eigenvectorsR[0][1]/eigenvectorsR[0][0]);
	double slopeG = GlobalDefines::abs<double>(eigenvectorsG[0][1]/eigenvectorsG[0][0]);
	double slopeB = GlobalDefines::abs<double>(eigenvectorsB[0][1]/eigenvectorsB[0][0]);

//	std::cout << "---------- redSlope: " << slopeR << ", greenSlope: " << slopeG
//		<< ", blueSlope: " << slopeB << std::endl;

	double minSlope = GlobalDefines::min<double>(
		GlobalDefines::min<double>(slopeR, slopeG), slopeB);
	double maxSlope = GlobalDefines::max<double>(
		GlobalDefines::max<double>(slopeR, slopeG), slopeB);


// TODO FIXME RAUSFINDEN, OB DAS PASST, DAS IST EIN GROESSERER EINGRIFF!
	if (cfg->fixEVFlaw) {
		// nur eine Steigung muss groesser sein als die Minimum-Steigung
		if ((maxSlope < cfg->minimum_eigenvector_slope) ||
			(maxSlope > cfg->maximum_eigenvector_slope)) {
			reportPatchQuality(patch_number,
				cfg->verbosity, true, true, true, true, false);
			if (cfg->verbosity > 0) std::cout << "slope = " << maxSlope << ", must be within " << cfg->minimum_eigenvector_slope << " and " << cfg->maximum_eigenvector_slope << std::endl;
			return false;
		}
	} else {
		// alle Steigungen muessen groesser sein als die Minimum-Steigung
		if ((minSlope < cfg->minimum_eigenvector_slope) ||
			(maxSlope > cfg->maximum_eigenvector_slope)) {
			reportPatchQuality(patch_number,
				cfg->verbosity, true, true, true, true, false);
			return false;
		}
	}

//	std::cout << "evR0 = " << eigenvaluesR[0][0] << ", evR1 = " << eigenvaluesR[0][1]
//		<< ", evG0 = " << eigenvaluesG[0][0] << ", evG1 = " << eigenvaluesG[0][1]
//		<< ", evB0 = " << eigenvaluesB[0][0] << ", evB1 = " << eigenvaluesB[0][1] << std::endl;

	double eccR = std::sqrt(1 - eigenvaluesR[0][1]/eigenvaluesR[0][0]);
	double eccG = std::sqrt(1 - eigenvaluesG[0][1]/eigenvaluesG[0][0]);
	double eccB = std::sqrt(1 - eigenvaluesB[0][1]/eigenvaluesB[0][0]);
	
	double minRatio = GlobalDefines::min<double>(
		GlobalDefines::min<double>(eccR, eccG), eccB);
//	std::cout << "  minRatio: " << minRatio << ", limit: " << cfg->minimum_eigenvalue_ratio << ";" << std::endl;

	if (minRatio < cfg->minimum_eigenvalue_ratio) {
		reportPatchQuality(patch_number,
			cfg->verbosity, true, true, true, true,
			true, false);
//		std::cerr << "check 1: soll-ecc = " << cfg->minimum_eigenvalue_ratio << ": " << eccR << "   " << eccG << "   " << eccR << std::endl;
		return false;
	}

	gamma[0] = meansR(0, 1) - (eigenvectorsR(0, 1) / eigenvectorsR(0, 0)) * meansR(0, 0);
	gamma[1] = meansG(0, 1) - (eigenvectorsG(0, 1) / eigenvectorsG(0, 0)) * meansG(0, 0);
	gamma[2] = meansB(0, 1) - (eigenvectorsB(0, 1) / eigenvectorsB(0, 0)) * meansB(0, 0);

	reportPatchQuality(patch_number, cfg->verbosity); // everything ok

	if (!cfg->no_plots) result_images->drawIid(diagR, diagG, diagB);

	return true;
}


/**** MOMENTAN UNGENUTZTER CODE, EINFACH IGNORIEREN! *****/
double illuminantEstimationByVoting::getConfidence(
	std::vector<cv::Vec3d > &pixels,
	unsigned int patch_number,
	cv::Vec3d &slope, cv::Vec3d &ecc,
	std::vector<std::vector<double> > &means,
	std::vector<std::vector<double> > &eigenvals, std::vector<std::vector<double> > &eigenvecs,
	cv::Vec3d &gamma
)
{
	if (pixels.size() < cfg->min_pixels) return 0;
	eigenvals.clear(); eigenvecs.clear(); means.clear();
	// red, green and blue diagram (chroma - inverse intensity)
	std::vector<double> intensitySum;
	PixelOperations::getVectorPixelSum(pixels, intensitySum);
	std::vector<cv::Vec3d> pixels_chrom;
	PixelOperations::getChromaticitiesVector<double>(pixels, pixels_chrom);
	cv::Mat_<double> diag;
	for (int c = 0; c < 3; ++c) {
		// note that channels are not in openCV order here
		MathOperations::intSumAndChroma2Mat<double>(intensitySum, pixels_chrom, c, diag);
		cv::Mat_<double> eigenvectors, eigenvalues, meansMat;

		// means are important for later...
		MathOperations::colMeans<double>(diag, meansMat);
		MathOperations::customPCA(diag, eigenvectors, eigenvalues);

		slope[c] = GlobalDefines::abs<double>(eigenvectors[0][1]/eigenvectors[0][0]);
		if (eigenvectors[0][0] < 0.000001) {
			ecc[c] = 0;
		} else {
			ecc[c] = std::sqrt(1 - eigenvalues[0][1]/eigenvalues[0][0]);
		}
		gamma[c] = meansMat(0, 1) - (eigenvectors(0, 1) / eigenvectors(0, 0)) * meansMat(0, 0);
		eigenvals.push_back(std::vector<double>() );
		eigenvecs.push_back(std::vector<double>() );
		means.push_back(std::vector<double>() );
		eigenvals[c].push_back(eigenvalues[0][0]);
		eigenvals[c].push_back(eigenvalues[0][1]);
		eigenvecs[c].push_back(eigenvectors[0][0]);
		eigenvecs[c].push_back(eigenvectors[0][1]);
		eigenvecs[c].push_back(eigenvectors[1][0]);
		eigenvecs[c].push_back(eigenvectors[1][1]);
		means[c].push_back(meansMat(0, 0)); means[c].push_back(meansMat(0, 1));
	}

	double minSlope = GlobalDefines::min<double>(
		GlobalDefines::min<double>(slope[0], slope[1]), slope[2]);
	double maxSlope = GlobalDefines::max<double>(
		GlobalDefines::max<double>(slope[0], slope[1]), slope[2]);

// TODO FIXME RAUSFINDEN, OB DAS PASST, DAS IST EIN GROESSERER EINGRIFF!
	if (cfg->fixEVFlaw) {
		if ((maxSlope < cfg->minimum_eigenvector_slope) ||
			(maxSlope > cfg->maximum_eigenvector_slope)) {
			reportPatchQuality(patch_number,
				cfg->verbosity, true, true, true, true, false);
			for (int i = 0; i < 3; ++i) gamma[i] = 0;
			return -1;
		}
	} else {
		if ((minSlope < cfg->minimum_eigenvector_slope) ||
			(maxSlope > cfg->maximum_eigenvector_slope)) {
			reportPatchQuality(patch_number,
				cfg->verbosity, true, true, true, true, false);
			for (int i = 0; i < 3; ++i) gamma[i] = 0;
			return -2;
		}
	}

	double minRatio = GlobalDefines::min<double>(
		GlobalDefines::min<double>(ecc[0], ecc[1]), ecc[2]);

	if (minRatio < cfg->minimum_eigenvalue_ratio) {
		reportPatchQuality(patch_number,
			cfg->verbosity, true, true, true, true,
			true, false);
		for (int i = 0; i < 3; ++i) gamma[i] = 0;
		return -3;
	}
	
	return 1;
}

/** basically a clone of ourNewMethodMainCheck
 * 
 */
bool illuminantEstimationByVoting::getPatchEstimate(
	std::vector<cv::Vec3d> &pixels,
	unsigned int patch_number,
	cv::Vec3d &gamma)
{
	// do the stuff that is in ourNewMethodMainCheck
	if (pixels.size() < cfg->min_pixels) {
		// complain, continue
		if (cfg->verbosity > 1) std::cout << "pixels.size() = " << pixels.size() << ", cfg->min_pixels = " << cfg->min_pixels << std::endl;
		reportPatchQuality(patch_number, cfg->verbosity, true, false);
		return false;
	}

	std::vector<cv::Vec3d> pixels_chrom;
	PixelOperations::getChromaticitiesVector<double>(pixels, pixels_chrom);
	std::vector<double> intensitySum;
	PixelOperations::getVectorPixelSum(pixels, intensitySum);

	if (cfg->method == "tan") {
		return tanEstimate(patch_number, intensitySum, pixels_chrom, gamma);
	}

	// methods that are not only eigenvalue-based do the heuristic checks
	if ((cfg->method.compare("ev_fhs")==0) && (cfg->method.compare("ev_adhoc")==0)) {

		if (!adhocChecks(pixels, pixels_chrom, intensitySum, patch_number)) return false;
	}

	if (cfg->verbosity > 1) std::cout << "EV-Checks:" << std::endl;
	return eigenvalueChecks(patch_number, intensitySum, pixels_chrom, gamma);
}

bool illuminantEstimationByVoting::tanEstimate(
        unsigned int           patch_number,
		std::vector< double >  &intensitySum,
		std::vector<cv::Vec3d> &pixels_chrom,
        cv::Vec3d              &gamma
	)
{
	if (cfg->verbosity > 1) std::cout << "Tan:" << std::endl;
	int nPixels = pixels_chrom.size();
	int accu[100];
	// color channels
	for (int c = 0; c < 3; c++) {
		for (int i = 0; i < 100; ++i) accu[i] = 0;
		for (int i = 0; i < nPixels; ++i) {
			for (int j = i+1; j < nPixels; ++j) {
				if (intensitySum[i] == intensitySum[j]) continue; // catch division by zero;

				// compute line between two pixels and the intersection with the y-axis
				double x1 = 1.0 / intensitySum[i];
				double y1 = pixels_chrom[i][c];
				double x2 = 1.0 / intensitySum[j];
				double y2 = pixels_chrom[j][c];
				
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
			if (accu[i] > max) {
				max = accu[i];
				maxPos = i;
			}
		}
		gamma[c] = static_cast<double>(maxPos) / 100.0;
	}
	// normalize to 1
	double sum = gamma[0] + gamma[1] + gamma[2];
	for (int i = 0; i < 3; ++i) gamma[i] = gamma[i] / sum;
	return true;
}

void illuminantEstimationByVoting::reportPatchQuality(
	unsigned int patch_id,
	unsigned int verbosity,
	bool edges_ok,
	bool minimum_number_pixels_ok,
	bool chromaticity_std_dev_ok,
	bool intensity_std_dev_ok,
	bool inverse_intensity_range_ok,
	bool eigenvector_slope_ok,
	bool eccentricity_ok)
{
	if (verbosity < 1) return;
	if (verbosity > 1){
		if( edges_ok && minimum_number_pixels_ok && chromaticity_std_dev_ok &&
			intensity_std_dev_ok && inverse_intensity_range_ok && eigenvector_slope_ok &&
			eccentricity_ok ){
			std::cout << "patch accepted: " << std::endl;
		} else {
			std::cout << "patch rejected: " << std::endl;
		}
	}

	std::cout << "Patch " << patch_id << ":" << // std::endl <<
		"  edges: " << edges_ok;
	if (!edges_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", min. num. pixels: " << minimum_number_pixels_ok;
	if (!minimum_number_pixels_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", max chrom. std. dev: " << chromaticity_std_dev_ok;
	if (!chromaticity_std_dev_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", min. intensity std. dev: " << intensity_std_dev_ok;
	if (!intensity_std_dev_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", min. x axis elongation: " << inverse_intensity_range_ok;
	if (!inverse_intensity_range_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", eigenvector slope: " << eigenvector_slope_ok;
	if (!eigenvector_slope_ok) {
		std::cout << std::endl;
		return;
	}
	std::cout << ", eccentricity: " << eccentricity_ok;
	if (!eccentricity_ok) {
		std::cout << std::endl;
		return;
	}

	std::cout << std::endl;
}

