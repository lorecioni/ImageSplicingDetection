#include "config_iebv.h"

using namespace boost::program_options;

namespace iic {

ConfigIebv::ConfigIebv(const std::string& prefix)
	  : Config(prefix),
		seg_felz(prefix + "_seg_felz"),
		seg_veks(prefix + "_seg_veks"),
		seg_patches(prefix + "_seg_patches"),
		config_histogram_voting_fusion(prefix + "_histogram_voting_fusion")
{
	initBoostOptions();

	excluded_area = NULL;
}

ConfigIebv& ConfigIebv::operator=(const vole::Config& other)
{
	// copy member variables of vole::Config to this
	verbosity = other.verbosity;
	prefix = other.prefix;
	prefix_enabled = other.prefix_enabled;
	seg_felz.prefix = other.prefix + "_seg_felz";
	seg_felz.prefix_enabled = other.prefix_enabled;
	seg_veks.prefix = other.prefix + "_seg_veks";
	seg_veks.prefix_enabled = other.prefix_enabled;
	seg_patches.prefix = other.prefix + "_seg_patches";
	seg_patches.prefix_enabled = other.prefix_enabled;
	config_histogram_voting_fusion.prefix = other.prefix + "_histogram_voting_fusion";
	config_histogram_voting_fusion.prefix_enabled = other.prefix_enabled;

	// FIXME: should we do that init here or not?
//	// init boost options
//	initBoostOptions();

	// set specific member variables
	excluded_area = NULL;

	return *this;
}

void ConfigIebv::initBoostOptions() {
	options.add_options()
		// global section
		(key("graphical"), bool_switch(&isGraphical)->default_value(false),
			 "Show any graphical output during runtime")
		;

// --- parameters for the pre-segmentation (felzenszwalb/huttenlocher)
	options.add_options()
		// FIXME: move this variable in seg_patches.options
		(key("seg_method"), value(&seg_method)->default_value("felzenszwalb"),
				   "Segmentation method used by the patches-segmentation.");
	options.add(seg_felz.options);
	options.add(seg_veks.options);
	options.add(seg_patches.options);

// --- parameters for the histogram_voting_fusion
	options.add( config_histogram_voting_fusion.options );

	options.add_options()
// --- parameters for the evaluation
		(key("mask_file"), value(&mask_file)->default_value(""),
		                   "file name for a binary mask file (for excluding pixels)")

// --- parameters for the automated algorithm 
		(key("number_patches"), value(&number_superpixels)->default_value(200),
		                   "Number of required patches for the voting scheme")
		(key("random_seed"), value(&random_seed)->default_value(0),
		                   "Seed for the random number generator")
		(key("with_hough"), bool_switch(&with_hough)->default_value(false),
		                   "Use Tan's Hough transform-based estimation in IIC space")

// parameters for quality check (applies to all variants of iebv)
		(key("max_intensity"), value(&max_intensity)->default_value(252.0/255.0, "252/255"),
		                   "Maximum intensity for a pixel")
		(key("min_intensity"), value(&min_intensity)->default_value(30.0/255.0, "30/255"),
		                   "Minimum intensity for a pixel")
		(key("min_pixels"), value(&min_pixels)->default_value(30),
		                   "Minimum number of useful pixels in a patch")
//		(key("remove_duplicate_pixels"), value(&remove_duplicate_pixels)->default_value(true),
//				   "Remove duplicate pixel colors when performing the patch check")

// parameters for cluster quality check (applies to "ev_*"-variants of iebv, esp. fhs)
		(key("minimum_eigenvalue_ratio"), value(&minimum_eigenvalue_ratio)->default_value(0.2, "0.2"),
		                   "Minimum ratio of eigenvalues")
		(key("minimum_eigenvector_slope"), value(&minimum_eigenvector_slope)->default_value(0.003, "0.003"),
		                   "Minimum slope of eigenvector")
		(key("maximum_eigenvector_slope"), value(&maximum_eigenvector_slope)->default_value(10.0, "10.0"),
		                   "Maximum slope of eigenvector")

// parameters for patch quality check (applies to "adhoc" variants of iebv)
//		(key("fixEVFlaw"), value(&fixEVFlaw)->default_value(true),
//				   "repairs minimum ev boundary (introduced before ECCV'10)")
		;

	if (prefix_enabled) return;
	options.add_options()
		(key("i16,J"), value(&input_file_16_bit)->default_value(""),
				   "16 bit RGB input image")
		(key("iintMax"), value(&input_intensity_max)->default_value(-1),
				   "Maximum input intensity")
		(key("input,I"), value(&input_file)->default_value(""),
				   "8 bit RGB input image")
		(key("output,O"), value(&output_directory)->default_value("/tmp/"),
				   "Working directory")
	;
}


std::string ConfigIebv::getString() const {
	std::stringstream s;
	if (prefix_enabled)	{
		s << "[" << prefix << "]" << std::endl;
	} else {
		s	<< "input=" << input_file << " # Image to process" << std::endl
			<< "i16=" << input_file_16_bit << " # 16 bit RGB input image" << std::endl
			<< "output=" << output_directory << " # Working directory" << std::endl;
	}
	
	s	<< "graphical=" << isGraphical
			<< " # Show any graphical output during runtime" << std::endl
		<< "seg_method=" << seg_method
			<< " # Segmentation method used by the patches-segmentation." << std::endl
		<< "mask_file=" << mask_file
			<< " # file name for a binary mask file (for excluding pixels)" << std::endl
		<< "number_patches=" << number_superpixels
			<< " # Number of required patches for the voting scheme" << std::endl
		<< "random_seed=" << random_seed
			<< " # Seed for the random number generator" << std::endl
		<< "with_hough=" << (with_hough ? 1 : 0)
			<< " # Use Tan's Hough transform-based estimation in IIC space" << std::endl
		<< "max_intensity=" << max_intensity
			<< " # Maximum intensity for a pixel" << std::endl
		<< "min_intensity=" << min_intensity
			<< " # Minimum intensity for a pixel" << std::endl
		<< "min_pixels=" << min_pixels
			<< " # Minimum number of useful pixels in a patch" << std::endl
		<< "minimum_eigenvalue_ratio=" << minimum_eigenvalue_ratio
			<< " # Minimum ratio of eigenvalues" << std::endl
		<< "minimum_eigenvector_slope=" << minimum_eigenvector_slope
			<< " # Minimum slope of eigenvector" << std::endl
		<< "maximum_eigenvector_slope=" << maximum_eigenvector_slope
			<< " # Maximum slope of eigenvector" << std::endl
		;
	s << seg_felz.getString();
	s << seg_veks.getString();
	s << seg_patches.getString();
	s << config_histogram_voting_fusion.getString();
	return s.str();
}


void ConfigIebv::setRandomSeed() const {
	unsigned int my_random_seed = 0;

	// use random seed from file?
	if (random_seed == 0) { // reading the seed file can fail, so check again if we have a seed
		my_random_seed = static_cast<unsigned int>(time(NULL) + rand());
	} else {
		my_random_seed = random_seed;
	}

	if (verbosity > 0)
		std::cout << "random seed: " << my_random_seed << std::endl;
	else
		std::cout << my_random_seed << " ";

	srand(my_random_seed); 
}

}
