#include "iebv.h"
#include "common/mask.h"
#include "fusion/histogramVoting/histogramVotingFusion.h"
#include "global_defines.h"
#include "iic.h"

#include <algorithm>

// debug includes, deletable for release:
#include "highgui.h"

using namespace illumestimators;

namespace iic {

/** Our variant of the estimator for the illuminant color
 */
Iebv::Iebv(const ConfigIebv &config)
	: config(config), estimator(IebvEstimator(config)), iic_space_set(false)
{
	// wow, two confessions in one line
	const_cast<ConfigIebv&>(const_cast<IebvEstimator&>(estimator).config).verbosity = 0;
}

Iebv::Iebv(const ConfigIebv &config, const cv::Mat_<cv::Vec4d> iic_space)
	: config(config), estimator(IebvEstimator(config)), iic_space(iic_space), iic_space_set(true)
{
	// wow, two confessions in one line
	const_cast<ConfigIebv&>(const_cast<IebvEstimator&>(estimator).config).verbosity = 0;
}

void Iebv::createIICSpace(cv::Mat_<cv::Vec3d> &image)
{
	IIC iic((cv::Mat_<cv::Vec3d>)image /*, config.min_intensity, config.max_intensity*/);
	iic_space = iic.getIIC();
	iic_space_set = true;
}

/** Estimate the dominant illuminant of an image using Lehmann/Palm specularity segmentation.
 *  \param image Input image in BGR format.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Iebv::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image) const
{
	// note: a patch in the old code is now a superpixel. Hence, we create a list of
	// superpixels and iterate over it. For each superpixel, we call est.illum(image, superpixel),
	// repeatedly, and collect the results. On these results, we then vote.
	if( !iic_space_set ) { return Illum(); }

	// preprocess the image, i.e. create mask to remove saturated and dark pixels
	cv::Mat_<unsigned char> mask_saturated_dark;
	cv::Mat_<cv::Vec3d> image_preprocessed;
	mask_saturated_dark = cv::Mat_<unsigned char>::zeros( image.rows, image.cols );
	this->preprocessImage( image, mask_saturated_dark, image_preprocessed, mask_saturated_dark );

	int number_tries_left = config.seg_patches.number_tries;
	std::vector< illumestimators::Illum > estimates;

	// main loop:
	//	1. create a single patch, randomly
	//	2. call our iebv_estimator for this single patch, returning an Illum estimate for it.
	//	3. if that estimate is valid, add if to our vector of estimates
	while( ((int)estimates.size() < config.number_superpixels) && (number_tries_left--) ){
		// select an image region, aka patch
		superpixels::Superpixel patch = selectPatchRandomly( image_preprocessed, config.seg_patches.size_x, config.seg_patches.size_y, &mask_saturated_dark );
		int patch_no = config.seg_patches.number_tries - number_tries_left;

		if( config.verbosity > 0 ){
			std::cout << "Patch " << patch_no <<": x = " << patch.bbox.x << ", y = " << patch.bbox.y << " (width: " << patch.bbox.width << ", height: " << patch.bbox.height << ", num_pixels: " << patch.coordinates.size() << ")" << std::endl << "        ";
		}

		Illum estimate = const_cast<IebvEstimator&>(estimator).estimate( iic_space, patch );

		// only if we get a valid result, we add it to our vector of estimates
		if( estimate.isValid ){
			estimates.push_back( estimate );
		}

		// output
		if( config.verbosity > 0 ){
			std::cout << "        "; // for nicer alignment of output
			int error = const_cast<IebvEstimator&>(estimator).error();
			std::cout << "min. num. pixels:: "     << ((error & 1) ? "0, " : "1, ");
			std::cout << "eigenvector slope: "   << ((error & 2) ? "0, " : "1, ");
			std::cout << "eccentricity: " << ((error & 4) ? "0 "  : "1 " );
			std::cout << std::endl;
		}
	}

	if( config.verbosity > 0 ){
		std::cout << "number of patches used for voting: " << estimates.size() << std::endl;
	} else {
		std::cout << estimates.size() << " ";
	}

	// vote: create a histogram voting and call it with the estimate vector. Return result from histogram.
	HistogramVotingFusion histogram_voting( config.config_histogram_voting_fusion );
	return histogram_voting.estimateIlluminant( estimates );

// original Plan by Christian Riess before first implementation by Manuel Meyer:
	// create patches,
	// intersect w/ superpixels,
	// call est.illum(image, superpixel, mask) repeatedly,
	// vote
}

/** Estimate the dominant illuminant of an image using a specularity mask.
 *  \param image Input image in BGR format.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Iebv::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const
{
	std::cerr << "not yet implemented: ";
	std::cerr << "Illum Iebv::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const";
	std::cerr << std::endl;

	if (!iic_space_set) { return Illum(); }

	// create patches,
	// intersect w/ superpixels,
	// call est.illum(image, superpixel, mask) repeatedly,
	// vote
	return Illum();
}

/** Estimate the dominant illuminant of a superpixel of an image.
 *  \param image Input image in BGR format.
 *  \param superpixel Image coordinates of the superpixel.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Iebv::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const superpixels::Superpixel& superpixel) const
{
	if (!iic_space_set) { return Illum(); }

	// one projection in iic space, one estimate
	return const_cast<IebvEstimator&>(estimator).estimate(iic_space, superpixel);
}

/** Estimate the dominant illuminant of a superpixel of an image.
 *  \param image Input image in BGR format.
 *  \param superpixel Image coordinates of the superpixel.
 *  \param mask Image mask.
 *  \return Illuminant chromaticities vector in BGR format.
 */
Illum Iebv::estimateIlluminant(const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask) const
{
	if (!iic_space_set || (superpixel.size() < 1)) { return Illum(); }
	// intersect pixels w/ mask,
	int pcount = 0;
	superpixels::Superpixel sp;

	bool one_projection = false;
	Illum estimate;

	if (one_projection) {
		sp.coordinates = std::vector<cv::Point>(superpixel.size());
		for (unsigned int i = 0; i < superpixel.size(); ++i) {
			if (mask[superpixel[i].y][superpixel[i].x] == 255) continue;
			sp.coordinates[pcount] = superpixel[i];
			pcount++;
		}
		if (pcount < (int)config.min_pixels) return Illum();
		sp.coordinates.resize(pcount);
	
		estimate = const_cast<IebvEstimator&>(estimator).estimate( iic_space, sp );
	} else { // multiple projections + voting
		// overlay superpixel with rectangular grid (grid size 20x20, in a distance of 10 pixels)
		// find out bounding box of superpixel
		int minX = image.cols; int minY = image.rows;
		int maxX = -1;         int maxY = -1;
		for (unsigned int i = 0; i < superpixel.size(); ++i) {
			if (mask[superpixel[i].y][superpixel[i].x] == 255) continue;
			if (superpixel[i].x > maxX) maxX = superpixel[i].x;
			if (superpixel[i].x < minX) minX = superpixel[i].x;
			if (superpixel[i].y > maxY) maxY = superpixel[i].y;
			if (superpixel[i].y < minY) minY = superpixel[i].y;
		}
		if ((minX > maxX) || (minY > maxY)) return Illum();

		int yBins = ((maxY-minY) / 10)+1;
		int xBins = ((maxX-minX) / 10)+1;
		superpixels::Superpixel **grid = new superpixels::Superpixel*[yBins];
		for (int y = 0; y < yBins; ++y) {
			grid[y] = new superpixels::Superpixel[xBins];
			for (int x = 0; x < xBins; ++x)
				grid[y][x] = superpixels::Superpixel();
		}

		for (unsigned int i = 0; i < superpixel.size(); ++i) {
			if (mask[superpixel[i].y][superpixel[i].x] == 255) continue;
			const cv::Point &p = superpixel[i];
			// every superpixel gets inserted in 1-4 bins
			int gridX = (p.x-minX) / 10;
			int gridY = (p.y-minY) / 10;
			grid[gridY][gridX].coordinates.push_back(p);
			if ((gridX > 0) && (gridY > 0)) grid[gridY-1][gridX-1].coordinates.push_back(p);
			if  (gridX > 0)                 grid[gridY  ][gridX-1].coordinates.push_back(p);
			if                 (gridY > 0)  grid[gridY-1][gridX  ].coordinates.push_back(p);
		}
		std::vector< illumestimators::Illum > estimates;
		for (int y = 0; y < yBins; ++y) {
			for (int x = 0; x < xBins; ++x) {
				estimate = const_cast<IebvEstimator&>(estimator).estimate( iic_space, grid[y][x] );
				if( estimate.isValid ) {
					estimates.push_back( estimate );
				}
			}
		}
		HistogramVotingFusion histogram_voting( config.config_histogram_voting_fusion );
		estimate = histogram_voting.estimateIlluminant( estimates );

		for (int y = 0; y < yBins; ++y) delete[] grid[y];
		delete[] grid;
	}

	return estimate;
}

void Iebv::preprocessImage(const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask, cv::Mat_<cv::Vec3d> &outputImage, cv::Mat_<unsigned char> &outputMask) const
{
	outputImage = image.clone();
	if ((image.rows != mask.rows) || (image.cols != mask.cols)) {
		std::cerr << "No mask!" << std::endl;
		outputMask = cv::Mat_<unsigned char>::zeros(outputImage.rows, outputImage.cols);
	} else {
		outputMask = mask.clone();
	}

	// DEBUG OUTPUT
//	for( int y = 319; y < 319+10; y++ ){
//		std::cout << "y = " << y << ":" << std::endl;
//		for( int x = 665; x < 665+40; x++ ){
//			std::cout << "    x = " << x << ": ";
//			std::cout << "(" << outputImage[y][x][0] << ", " << outputImage[y][x][1] << ", " << outputImage[y][x][2] << ")" << std::endl;
//		}
//		std::cout << std::endl;
//	}
//	std::cout << std::endl;
	// DEBUG OUTPUT END

	illumestimators::Mask::maskSaturatedPixels(outputImage, outputMask, config.max_intensity);
	illumestimators::Mask::maskMinPixels(outputImage, outputMask, config.min_intensity);
}

/* no machine learning involved - no saving of training data */
bool Iebv::save(const std::string&) const
{
	return true;
}

/* no machine learning involved - no loading of training data */
bool Iebv::load(const std::string&)
{
	return true;
}


/** No machine learning involved - no training with labelled images.
 *  \param imageFiles Filenames of training images.
 *  \param colorspaces Colorspaces of training images.
 *  \param illuminants Illuminant chromaticities vectors of training images.
 *  \param maskFiles Filenames of mask images.
 */
bool Iebv::train(
	const std::vector<std::string>& imageFiles,
	const std::vector<std::string>& colorspaces,
	const std::vector<cv::Vec3d>& illuminants,
	const std::vector<std::string>& maskFiles)
{
	return true;
}

int Iebv::error() {
	return estimator.error();
}

/** Name of illuminant estimator with current parameters for display purposes.
 *  \return Name of illuminant estimator.
 */
std::string Iebv::name() const
{
	return std::string("Voting in IIC space (IEBV)");
}

/** Unique identifier of illuminant estimator given current parameters.
 *  \return Unique identifier of illuminant estimator.
 */
std::string Iebv::identifier() const
{
	return std::string("iebv");
}

/////////////////////////////////////
//   Protected member functions:   //
/////////////////////////////////////
/** randomly selects a patch region within the given image.
 *  \param image Input image in BGR format.
 *  \param width defines the width of the patch
 *  \param height defines the height of the patch
 *  \param mask an optinal mask: black pixels will be ignored. Mask marks e.g. too saturated pixels, see preprocessImage().
 *  \param excluded_area an optional mask: in it's black area, not more than 50% of the patch's pixel
 *                       will lie. Currently as pointer, maybe as reference later...
 *  \return a vector of OpenCV Point structures, defining the patch
 */
superpixels::Superpixel Iebv::selectPatchRandomly(
	const cv::Mat_<cv::Vec3d> &image,
	int width,
	int height,
	const cv::Mat_<unsigned char>* mask,
	const cv::Mat_<unsigned char>* excluded_area ) const
{
	// sanity check: only if image exists, we continue
	assert( (image.rows > 0) && (image.cols > 0) );

	superpixels::Superpixel patch;
	int w = width;
	int h = height;
	int x0 = 0;
	int y0 = 0;

	while( true )
	{
		// randomly choose if patch shall be horizontal or vertical oriented
		unsigned int horiz_vert = static_cast<unsigned int>(GlobalDefines::abs<int>(rand()));
		if ((horiz_vert % 2) == 1) {
			std::swap<int>(h, w);
		}
		// check if width and height still fit the image's size
		if( image.cols <= w ){ // if the patch's width is too big
			if( config.verbosity > 0 ) { // print warning
				std::cerr << "Warning: image width is only " << image.cols << " but patch width was set to"
					  << w << ". Maximizing it but results from such a patch size won't be very meaningful."
					  << std::endl;
			}
			x0 = 0;
			w = image.cols - 1;
		} else { // select upperleft x-coordinate x0 randomly
			x0 = static_cast<unsigned int>(GlobalDefines::abs<int>(rand())) % (image.cols - w);
		}
		if( image.rows <= height ){ // if the patch's height is too big
			if( config.verbosity > 0 ) { // print warning
				std::cerr << "Warning: image height is only " << image.rows << " but patch height was set to"
					  << h << ". Maximizing it but results from such a patch size won't be very meaningful."
					  << std::endl;
			}
			y0 = 0;
			h = image.rows - 1;
		} else { // select upperleft y-coordinate y0 randomly
			y0 = static_cast<unsigned int>(GlobalDefines::abs<int>(rand())) % (image.rows - h);
		}

		// if no excluded_area was specified, we can exit
		if( excluded_area == 0 ){
			break;
		}

		// else we need to test our current pick
		unsigned int rightX = x0 + w;
		unsigned int lowerY = y0 + h;
		unsigned int count = 0;
		for( unsigned int y = y0; y < lowerY; y++ ){
			const unsigned char* cur_row = (*excluded_area)[y];
			for( unsigned int x = x0; x < rightX; x++ ){
				if( cur_row[x] > 127 ) count++;
			}
		}
		if( (int)count >= (w*h)/2) break; // ok, more than 50% of the pixels can be used
	}

	patch.bbox = cv::Rect( x0, y0, w, h );

	int y1 = y0 + h;
	int x1 = x0 + w;
	for( int y = y0; y < y1; y++ ){
		for( int x = x0; x < x1; x++ ){
			// if our mask is not NULL we need to test wether to ignore this position or not
			if( mask != 0 ){
				if( (*mask)[y][x] != 0 ){
					continue;
				}
			}
			patch.coordinates.push_back( cv::Point(x, y) );
		}
	}

	patch.size = patch.coordinates.size();

	return patch;
}

} // namespace iic

