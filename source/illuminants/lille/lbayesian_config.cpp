#include "lbayesian_config.h"

using namespace boost::program_options;

namespace lille {

LBayesianConfig::LBayesianConfig(const std::string& prefix) :
	Config(prefix), img("img")
{
	#ifdef WITH_BOOST
		initBoostOptions();
	#endif // WITH_BOOST
}

std::string LBayesianConfig::getString() const
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

	  << "n=" << n << " # Derivative order" << std::endl
	  << "p=" << p << " # Parameter of Minkowski norm" << std::endl
	  << "sigma=" << sigma << " # Standard deviation of Gaussian smoothing filter" << std::endl
	  << img.getString()
	;

	return s.str();
}

#ifdef WITH_BOOST
void LBayesianConfig::initBoostOptions()
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

		(key("n"), boost::program_options::value(&n)->default_value(0), "Derivative order (0=gw,>0: gray edge)")
		(key("p"), boost::program_options::value(&p)->default_value(1), "Parameter of Minkowski norm (1=gw,-1=maxRGB,>1:ggw")
		(key("sigma"), boost::program_options::value(&sigma)->default_value(0), "Standard deviation of Gaussian smoothing filter")
	;
}
#endif // WITH_BOOST

} // namespace lille

