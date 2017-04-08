/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_PATCHES_PATCHESSEGMENTATION_H
#define SUPERPIXELS_PATCHES_PATCHESSEGMENTATION_H

#include "superpixelsegmentation.h"
#include "commands/patchesconfig.h"

namespace superpixels {

class PatchesSegmentation : public SuperpixelSegmentation
{
public:
	/** Constructor for a given config, a given mask, and a given exclusion area. The caller must take care of the mask's and the excluded area's pointers.
	 * \param config a config of type vole::PatchesConfig that contains the patches specific options.
	 * \param mask a mask for pixels that shall be excluded from patch creation, e.g. too saturated or too dark pixels. Caller must take care of the given pointer.
	 * \param excluded_area an area marked for exclusion, e.g. from a specific object in the image, like a ball. Caller must take care of the given pointer.
	 */
	PatchesSegmentation( vole::PatchesConfig config, const cv::Mat_<unsigned char>* mask = 0, const cv::Mat_< unsigned char >* excluded_area = 0 );
	/** Constructor for a given config, a given segmenter, a given mask, and a given exclusion area. The caller must take care of the mask's and the excluded area's pointers.
	 * \param config a config of type vole::PatchesConfig that contains the patches specific options.
	 * \param segmenter a general, real segmentation method, e.g. Felzenszwalb. The class will take care of this pointer, i.e. delete it in its destructor.
	 * \param mask a mask for pixels that shall be excluded from patch creation, e.g. too saturated or too dark pixels. Caller must take care of the given pointer.
	 * \param excluded_area an area marked for exclusion, e.g. from a specific object in the image, like a ball. Caller must take care of the given pointer.
	 */
	PatchesSegmentation( vole::PatchesConfig config, SuperpixelSegmentation* segmenter, const cv::Mat_<unsigned char>* mask = 0, const cv::Mat_< unsigned char >* excluded_area = 0 );
	/** Destructor will delete the member variables, i.e. the given SuperpixelSegmentation* segmenter, provided in constructor or setSegmenter( SuperpixelSegmentation* segmenter );
	 */
	~PatchesSegmentation();

public:
	/** Function to set the internal concrete segmenter class, e.g. FelzenszwalbSegmentation.
	 * \param segmenter a pointer to an abstract SuperpixelSegmentation object. The class will take care of the pointer, i.e. delete it in its destructor.
	 */
	void setSegmenter( SuperpixelSegmentation* segmenter );

	std::vector<Superpixel> useSegmenter( const cv::Mat_<cv::Vec3b>& image );
	std::vector<Superpixel> useSegmenter( const cv::Mat_<cv::Vec3d>& image );

	std::vector<Superpixel> superpixels( const cv::Mat_<cv::Vec3b>& image );
	std::vector<Superpixel> superpixels( const cv::Mat_<cv::Vec3d>& image );

	int sizeSegmenterSuperpixels() const { return segmenter_superpixels_.size(); }

#define GPR_VERSION 1
#if GPR_VERSION==1
	void getPatchRandomly( int rows, int cols, Superpixel& patch );
#elif GPR_VERSION==2
	// DEBUG
	Superpixel getPatchRandomly();
	void setImage( cv::Mat_<cv::Vec3b> image ) { image_ = image; };
	cv::Mat_<cv::Vec3b> image_;
#else
	void getPatchRandomly( const cv::Mat_<cv::Vec3b>& image, Superpixel& patch );
#endif
	// DEBUG END

private:
	SuperpixelSegmentation* segmenter_;
	bool has_segmenter_;
	std::vector< Superpixel > segmenter_superpixels_;
	bool has_segmentation_;
	vole::PatchesConfig *localCfg; 
	const cv::Mat_< unsigned char >* mask_;
	const cv::Mat_< unsigned char >* excluded_area_;

#if GPR_VERSION==1
	// DEBUG
	void intersect( int rows, int cols, Superpixel& patch );
	// DEBUG END
#else
	void intersect( const cv::Mat_< cv::Vec3b >& image, Superpixel& patch );
#endif

};

} // namespace superpixels

#endif // SUPERPIXELS_PATCHES_PATCHESSEGMENTATION_H
