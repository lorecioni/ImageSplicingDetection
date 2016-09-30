#ifndef IIC_CONFIG_IEBV
#define IIC_CONFIG_IEBV

#include <opencv2/imgproc/imgproc.hpp>

#include "config.h"
#include "commands/felzenszwalbconfig.h"
#include "commands/vekslerconfig.h"
#include "commands/patchesconfig.h"
#include "fusion/histogramVoting/config_histogram_voting_fusion.h"

namespace iic {

class ConfigIebv : public vole::Config {

public:
	ConfigIebv(const std::string& prefix = std::string());

	// TODO: const ConfigIebv& as parameter, not vole::Config& ?!
	ConfigIebv& operator=(const vole::Config& other);

	virtual std::string getString() const;
	void setRandomSeed() const;

protected:
	virtual void initBoostOptions();

public:
	/// may iebv create graphical output (i.e. open graphical windows)?
	bool isGraphical;
	/// input file name
	std::string input_file;
	std::string input_file_16_bit;
	int input_intensity_max;
	/// directory for all intermediate files
	std::string output_directory;
	// FIXME: move this variable to seg_patches
	std::string seg_method;

	// presegmentation parameters
	vole::FelzenszwalbConfig seg_felz;
	vole::VekslerConfig seg_veks;
	vole::PatchesConfig seg_patches;
	illumestimators::ConfigHistogramVotingFusion config_histogram_voting_fusion;

	// what pieces to use from the segmentation, {random, largest, all}
	// "random" picks any region that intersects with the patch
	// "largest" picks the largest region that intersects with the patch
	// "all" does not draw separate patches, but processes every segment in the image

	// parameters for the evaluation
	// evaluation of the ciurea/funt data needs to mask out the ground truth ball
	std::string mask_file;
	cv::Mat_<unsigned char> *excluded_area;

	// --- parameters for the automated algorithm 
	int number_superpixels; // number of required good patches for the voting
	unsigned int random_seed; // seed for the random number generator
	bool with_hough; // use hough transform for estimation (Tan's method)

	// parameters for quality check in main quality function
	double max_intensity; // maximum intensity for a pixel
	double min_intensity; // minimum intensity for a pixel
	unsigned int min_pixels; // minimum number of useful pixels in a patch

	// --- parameters for cluster quality check
	double minimum_eigenvalue_ratio; // minimum ratio of eigenvalues
	double minimum_eigenvector_slope; // minimum slope of eigenvector
	double maximum_eigenvector_slope; // maximum slope of eigenvector


};

}

#endif // IIC_CONFIG_IEBV
