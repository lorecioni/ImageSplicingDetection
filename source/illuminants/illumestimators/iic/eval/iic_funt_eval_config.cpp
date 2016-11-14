#include "iic_funt_eval_config.h"
#include <sstream>

#ifdef WITH_BOOST_PROGRAM_OPTIONS
using namespace boost::program_options;
#endif

namespace iic {

FuntEvalConfig::FuntEvalConfig() : Config() 
{
	#ifdef WITH_BOOST_PROGRAM_OPTIONS
	initBoostOptions();
	#endif
}

std::string FuntEvalConfig::getString() const
{
	std::stringstream s;
	if (prefix_enabled) {
		s << "[" << prefix << "]" << std::endl;
	} else {
		s	<< "input=" << input_file << " # Image to process" << std::endl
			<< "output=" << output_directory << " # Working directory" << std::endl;
	}
	s	<< "verbose=" << verbosity
			<< " # Verbosity Level: 0 = silent, 1 = normal, 2 = much output, 3 = insane"<<std::endl
		<< "funt_root_dir=" << root_dir
			<< " # root directory w.r.t. the file list" << std::endl
		<< "funt_filelist=" << funt_filelist << " # file list" << std::endl
		<< "ground_truth=" << ground_truth << " # ground truth file" << std::endl
		<< "pix_per_set=" << pix_per_set
			<< " # selection of the images, set 0 for using all" << std::endl
		<< "iebv_config_file=" << iebv_config_file << " # config file for iebv" << std::endl
		<< "iebv_submethod=" << iebv_submethod
			<< " # {all, random, largest}, where all forces all segments to be used," << std::endl
			<< " # random picks randomly segments, largest picks the largest in one patch" << std::endl
			<< " # (only used for fhs-based variants" << std::endl
		<< "n_estimates=" << n_estimates << "number of estimates per image" << std::endl
	;
	return s.str();
}

#ifdef WITH_BOOST_PROGRAM_OPTIONS
void FuntEvalConfig::initBoostOptions() {
	options.add_options()

// --- parameters for the pre-segmentation (felzenszwalb/huttenlocher)
		(key("funt_root_dir"),
			value(&root_dir)->default_value("/net/cv/illum/ciurea_funt/", "cv/../funt"),
		                   "root directory w.r.t. the file list")
		(key("funt_filelist"),
			value(&funt_filelist)->default_value("/net/cv/illum/ciurea_funt/file.lst", "file.lst"),
		                   "file list")
		(key("ground_truth"),
			value(&ground_truth)->default_value("/net/cv/illum/ciurea_funt/funt_ground_truth.txt", "funt_ground_truth.txt"),
		                   "ground truth file")
		(key("iebv_config_file"),
			value(&iebv_config_file)->default_value("iebv_config.txt"),
		                   "config file for iebv")
		(key("iebv_submethod"),
			value(&iebv_submethod)->default_value(""),
			"{all, random, largest}, where all forces all segments to be used, random picks randomly segments, largest picks the largest in one patch (only used for fhs-based variants")
		(key("n_estimates"),
			value(&n_estimates)->default_value(5), "number of estimates per image")
		(key("pix_per_set"),
			value(&pix_per_set)->default_value(0), "number of images to select from each subset (0 = use all)")
	;
	if (prefix_enabled) return;
	options.add_options()
		(key("input,I"), value(&input_file)->default_value("input.png"),
		 "Image to process")
		(key("output,O"), value(&output_directory)->default_value("/tmp/"),
		 "Working directory")
	;
}

}
#endif // WITH_BOOST_PROGRAM_OPTIONS

