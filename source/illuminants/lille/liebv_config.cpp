#include "liebv_config.h"

using namespace boost::program_options;

namespace lille {

LIebvConfig::LIebvConfig(const std::string& prefix) :
	Config(prefix), img("img")
{
	#ifdef WITH_BOOST
		initBoostOptions();
	#endif // WITH_BOOST
}

std::string LIebvConfig::getString() const
{
	std::stringstream s;

	if (prefix_enabled) {
		s << "[" << prefix << "]" << std::endl;
	} else {
		s //<< "input=" << inputFile << " # Image to process" << std::endl
		  << "output=" << outputFile << " # Annotated illuminant-segmented file" << std::endl
		;
	}
	s << "segmentation=" << segmentationFile << " # Superpixel segmentation, stored in an image file" << std::endl
	  << "mask=" << maskFile << " # mask b&w image: all pixels with zero intensity are ignored"

	  << "iebv_config=" << iebvConfigFile
	  	<< " # configuration file for iebv" << std::endl
	  << img.getString()
	;

	return s.str();
}

#ifdef WITH_BOOST
void LIebvConfig::initBoostOptions()
{
	if (!prefix_enabled) {
		options.add_options()
			(key("output,O"), value(&outputFile)->default_value(""), "Annotated illuminant-segmented file")
		;
	}

	options.add(img.options);
	options.add_options()
		(key("segmentation,S"), value(&segmentationFile)->default_value(""), "Superpixel segmentation, stored in an image file")
		(key("mask"), value(&maskFile)->default_value(""), "mask b&w image: all pixels with zero intensity are ignored")
	    (key("iebv_config"), value(&iebvConfigFile)->default_value(""), "configuration file for iebv")
	;
}
#endif // WITH_BOOST

} // namespace lille

