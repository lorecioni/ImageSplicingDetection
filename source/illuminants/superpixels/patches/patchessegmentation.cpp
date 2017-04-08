/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "patchessegmentation.h"
#include "commands/patchesconfig.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/highgui/highgui.hpp>
#include <limits>
#include <cstdlib>

#include <time.h>

namespace superpixels
{
/** Constructor for a given config, a given mask, and a given exclusion area. The caller must take care of the mask's and the excluded area's pointers.
 * \param config a config of type vole::PatchesConfig that contains the patches specific options.
 * \param mask a mask for pixels that shall be excluded from patch creation, e.g. too saturated or too dark pixels. Caller must take care of the given pointer.
 * \param excluded_area an area marked for exclusion, e.g. from a specific object in the image, like a ball. Caller must take care of the given pointer.
 */
PatchesSegmentation::PatchesSegmentation(
	vole::PatchesConfig config,
	const cv::Mat_<unsigned char>* mask,
	const cv::Mat_< unsigned char >* excluded_area )
	: segmenter_( 0 ),
	  has_segmenter_( false ),
	  has_segmentation_( false ),
	  mask_( 0 ),
	  excluded_area_( 0 )
{
	mask_ = mask;
	excluded_area_ = excluded_area;

	// TODO: why a pointer here? Why not copy the config?
	localCfg = new vole::PatchesConfig();

	// TODO: cleanup: instead of copying by hand here, offer an operator=
	localCfg->size_x         = config.size_x;
	localCfg->size_y         = config.size_y;
	localCfg->number_tries   = config.number_tries;
	localCfg->strategy = config.strategy;

	this->config = localCfg;
}

/** Constructor for a given config, a given segmenter, a given mask, and a given exclusion area. The caller must take care of the mask's and the excluded area's pointers.
 * \param config a config of type vole::PatchesConfig that contains the patches specific options.
 * \param segmenter a general, real segmentation method, e.g. Felzenszwalb. The class will take care of this pointer, i.e. delete it in its destructor.
 * \param mask a mask for pixels that shall be excluded from patch creation, e.g. too saturated or too dark pixels. Caller must take care of the given pointer.
 * \param excluded_area an area marked for exclusion, e.g. from a specific object in the image, like a ball. Caller must take care of the given pointer.
 */
PatchesSegmentation::PatchesSegmentation(
	vole::PatchesConfig config,
	SuperpixelSegmentation* segmenter,
	const cv::Mat_< unsigned char >* mask,
	const cv::Mat_< unsigned char >* excluded_area )
	: SuperpixelSegmentation( *segmenter ),
	  segmenter_( segmenter ),
	  has_segmenter_( true ),
	  has_segmentation_( false )
{
	mask_ = mask;
	excluded_area_ = excluded_area;

	localCfg = new vole::PatchesConfig();
	localCfg->size_x         = config.size_x;
	localCfg->size_y         = config.size_y;
	localCfg->number_tries   = config.number_tries;
	localCfg->strategy = config.strategy;

	this->config = localCfg;
}

PatchesSegmentation::~PatchesSegmentation()
{
	delete segmenter_;
	segmenter_ = 0;
}

/** Function to set the internal concrete segmenter class, e.g. FelzenszwalbSegmentation.
 * \param segmenter a pointer to an abstract SuperpixelSegmentation object. The class will take care of the pointer, i.e. delete it in its constructer.
 */
void PatchesSegmentation::setSegmenter( SuperpixelSegmentation* segmenter )
{
	segmenter_ = segmenter;
	has_segmenter_ = true;
}

std::vector<Superpixel> PatchesSegmentation::useSegmenter( const cv::Mat_<cv::Vec3d>& image )
{
	cv::Mat_<cv::Vec3b> inputImage;
	image.convertTo( inputImage, 8, 255 );
	return useSegmenter( inputImage );
}

std::vector<Superpixel> PatchesSegmentation::useSegmenter( const cv::Mat_<cv::Vec3b>& image )
{
	// only perform time-intensive segmentation if strategy is not "all"
	if( localCfg->strategy.compare( "all" ) == 0 ){
		has_segmentation_ = false;
		return std::vector<Superpixel>();
	}

	if( !has_segmenter_ ) return std::vector<Superpixel>();
	segmenter_superpixels_ = segmenter_->superpixels( image );
	has_segmentation_ = true;
	segmenter_->superpixelsLabelImage( segmenter_superpixels_, image.rows, image.cols );
	return segmenter_superpixels_;
}

std::vector<Superpixel> PatchesSegmentation::superpixels( const cv::Mat_<cv::Vec3b>& image )
{

	if( has_segmentation_ ){
		return segmenter_superpixels_;
	}
	// else we segment the whole image using the given segmenter
	useSegmenter( image );

	// select localCfg->number_tries patches
	std::vector< Superpixel > patches;
	Superpixel patch;
	for( int i = 0; i < localCfg->number_tries; i++ ){
		getPatchRandomly( image.rows, image.cols, patch );

		patches.push_back( patch );
	}

	_superpixels = patches;

	return _superpixels;
}

std::vector<Superpixel> PatchesSegmentation::superpixels(const cv::Mat_<cv::Vec3d>& image)
{
	cv::Mat_<cv::Vec3b> inputImage;
	image.convertTo(inputImage, 8, 255);
	return superpixels(inputImage);
}

void PatchesSegmentation::getPatchRandomly( int rows, int cols, Superpixel& patch )
{
	// if patch is not empty, clear it
	if( !patch.empty() ){
		patch.clear();
	}

	// sanity check: only if image exists, we continue
	assert( (rows > 0) && (cols > 0) );

	int w = localCfg->size_x;
	int h = localCfg->size_y;
	int x0 = 0;
	int y0 = 0;

	while( true )
	{
		// randomly choose if patch shall be horizontal or vertical oriented
		unsigned int horiz_vert = static_cast<unsigned int>(abs(rand()));
		if ((horiz_vert % 2) == 1) {
			std::swap<int>(h, w);
		}
		// check if width and height still fit the image's size
		if( cols <= w ){ // if the patch's width is too big
			if( localCfg->verbosity > 0 ) { // print warning
				std::cerr << "Warning: image width is only " << cols << " but patch width was set to"
					  << w << ". Maximizing it but results from such a patch size won't be very meaningful."
					  << std::endl;
			}
			x0 = 0;
			w = cols - 1;
		} else { // select upperleft x-coordinate x0 randomly
			x0 = static_cast<unsigned int>(abs(rand())) % (cols - w);
		}
		if( rows <= height ){ // if the patch's height is too big
			if( localCfg->verbosity > 0 ) { // print warning
				std::cerr << "Warning: image height is only " << rows << " but patch height was set to"
					  << h << ". Maximizing it but results from such a patch size won't be very meaningful."
					  << std::endl;
			}
			y0 = 0;
			h = rows - 1;
		} else { // select upperleft y-coordinate y0 randomly
			y0 = static_cast<unsigned int>(abs(rand())) % (rows - h);
		}

		// if no excluded_area was specified, we can exit
		if( excluded_area_ == 0 ){
			break;
		}

		// else we need to test our current pick
		unsigned int x1 = x0 + w;
		unsigned int y1 = y0 + h;
		unsigned int count = 0;
		for( unsigned int y = y0; y < y1; y++ ){
			const unsigned char* cur_row = (*excluded_area_)[y];
			for( unsigned int x = x0; x < x1; x++ ){
				if( cur_row[x] > 127 ) count++;
			}
		}
		if( (int)count >= (w*h)/2) break; // ok, more than 50% of the pixels can be used
	}

	patch.bbox = cv::Rect( x0, y0, w, h );
	width = w;
	height = h;

	// now intersect the current patch with our segmented image
	if( has_segmentation_ ){
		intersect( rows, cols, patch );
	}
	else{
		// add all pixels in a patch to the patch - same as strategy = all
		for( int y = y0; y < y0 + h; y++ ){
			for( int x = x0; x < x0 + w; x++ ){
				// test if pixel is allowed by mask
				if( (mask_ == 0) || ((*mask_)[y][x] == 0) ){
					patch.coordinates.push_back( cv::Point(x, y) );
				}
			}
		}
		patch.size = patch.coordinates.size();
	}
}

//////////////////////////////////
//   private helper functions   //
//////////////////////////////////

void PatchesSegmentation::intersect( int rows, int cols, Superpixel& patch )
{
	std::string strategy = localCfg->strategy;
	// should never be the case, but for safety test if segmenter_ exists; if not, use all pixels of a patch
	if( !has_segmenter_ ){
		std::cout << "Warning in void PatchesSegmentation::intersect( Superpixel& patch, const cv::Mat_< cv::Vec3b >& image ): "
			  << "no segmenter was set in constructor or setSegmenter( SuperpixelSegmentation& segmenter ). Returning full patch, not intersection." << std::endl;
		strategy = "all";
	} else {
		// should never be the case, but for safety test if segmentation_ exists; if not, create it
		if( !has_segmentation_ ){
			std::cout << "Warning in void PatchesSegmentation::intersect( int rows, int cols, Superpixel& patch ): "
				  << "No segmentation available. \"std::vector<Superpixel> PatchesSegmentation::useSegmenter( const cv::Mat_<cv::Vec3b>& image )\" "
				  << "must be called before this function. Returning full patch, not intersection." << std::endl;
			strategy = "all";
		}
	}

	int x0 = patch.bbox.x;
	int y0 = patch.bbox.y;
	int x1 = patch.bbox.x + patch.bbox.width;
	int y1 = patch.bbox.y + patch.bbox.height;

	// depending on our strategy, we select a segment
	// ALL PIXELS WITHIN PATCH
	if( strategy.compare( "all" ) == 0 ){
		for( int y = y0; y < y1; y++ ){
			for( int x = x0; x < x1; x++ ){
				// test if pixel is allowed by mask
				if( (mask_ == 0) || ((*mask_)[y][x] == 0) ){
					patch.coordinates.push_back( cv::Point(x, y) );
				}
			}
		}
	}
	else {
		// preparation for other strategies
		// get label image
		cv::Mat_< int > & labels = segmenter_->superpixelsLabelImage( segmenter_superpixels_, rows, cols );

		// determine the unique components that intersect the current patch
		std::map< int, int > intersections; // map of segment label and according identifier
		int identifier = 0;
		for( int y = y0; y < y1; y++ ){
			for( int x = x0; x < x1; x++ ){
				// test if we have no excluded_area_ or it says the current pixel is okay
				if( (excluded_area_ == 0) || ((*excluded_area_)[y][x] > 127) ){
					// test if we found that label before
					if( intersections.find( labels[y][x] ) == intersections.end() ){
						intersections[ labels[y][x] ] = identifier;
						identifier++;
					}
				}
			}
		}

		// other strategies
		// RANDOM SEGMENT WITHIN PATCH
		if( strategy.compare( "random" ) == 0 ){
			// draw randomly a pixel from the component -> this component number is selected
			int label_x, label_y;
			// there must be pixels outside of the excluded area (condition on the patch selection)
			do {
				label_x = static_cast<unsigned int>(std::abs<int>(rand())) % (patch.bbox.width);
				label_y = static_cast<unsigned int>(std::abs<int>(rand())) % (patch.bbox.height);
			} while ( (excluded_area_ != 0) && ((*excluded_area_)[y0][x0] <= 127) );

			int random_label = labels[ y0 + label_y ][ x0 + label_x ];
			// find all pixels in this patch with this label
			for( int y = y0; y < y1; y++ ){
				for( int x = x0; x < x1; x++ ){
					// test if we have no excluded_area_ or it says the current pixel is okay
					if( (excluded_area_ == 0) || ((*excluded_area_)[y][x] > 127) ){
						// test if current pixel has the random_label
						if( labels[y][x] == random_label ){
							// test if pixel is allowed by mask
							if( (mask_ == 0) || ((*mask_)[y][x] == 0) ){
								patch.coordinates.push_back( cv::Point(x, y) );
							}
						}
					}
				}
			}
		}
		// LARGEST SEGMENT WITHIN PATCH
		else if( strategy.compare( "largest" ) == 0 ){
			// count the number of pixels per segment
			int segment_sizes[ identifier ];
			memset( segment_sizes, 0, sizeof(int)*identifier );
			for( int y = y0; y < y1; y++ ){
				for( int x = x0; x < x1; x++ ){
					// test if we have no excluded_area_ or it says the current pixel is okay
					if( (excluded_area_ == 0) || ((*excluded_area_)[y][x] > 127) ){
						segment_sizes[ intersections[ labels[y][x] ] ]++;
					}
				}
			}

			// find largest segment
			int id_largest = -1;
			int max_size = -1;
			for( int i = 0; i < identifier; i++ ){
				if( segment_sizes[ i ] > max_size ){
					max_size = segment_sizes[ i ];
					id_largest   = i;
				}
			}

			// grab all pixels with id_largest that intersect the patch
			for( int y = y0; y < y1; y++ ){
				for( int x = x0; x < x1; x++ ){
					// test if we have no excluded_area_ or it says the current pixel is okay
					if( (excluded_area_ == 0) || ((*excluded_area_)[y][x] > 127) ){
						// test if patch-position is of segment id_largest
						if( intersections[ labels[y][x] ] == id_largest ){
							// test if pixel is allowed by mask
							if( (mask_ == 0) || ((*mask_)[y][x] == 0) ){
								patch.coordinates.push_back( cv::Point(x, y) );
							}
						}
					}
				}
			}
		}
		else{
			std::cerr << "ERROR: strategy \"" << localCfg->strategy << "\" is illegal; should be one of {random, largest, all}"  << std::endl;
			exit(1);
		}
	}

	patch.size = patch.coordinates.size();
}

} // namespace superpixels
