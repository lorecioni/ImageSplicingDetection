#include "grayworldconfig.h"

namespace illumestimators {

GrayWorldConfig::GrayWorldConfig(const std::string& prefix) :
	Config(prefix)
{
	#ifdef WITH_BOOST
		initBoostOptions();
	#endif // WITH_BOOST
}

std::string GrayWorldConfig::getString() const
{
	std::stringstream s;

	if (prefix_enabled) {
		s << "[" << prefix << "]" << std::endl;
	} else {
		s << "input=" << inputFile << " # Image to process" << std::endl
		;
	}
	s << "n=" << n << " # Derivative order" << std::endl
	  << "p=" << p << " # Parameter of Minkowski norm" << std::endl
	  << "sigma=" << sigma << " # Standard deviation of Gaussian smoothing filter" << std::endl
	;

	return s.str();
}

#ifdef WITH_BOOST
void GrayWorldConfig::initBoostOptions()
{
	if (!prefix_enabled) {
		options.add_options()(key("input,I"), boost::program_options::value(&inputFile)->default_value(""), "Image to process");
	}

	options.add_options()(key("n"), boost::program_options::value(&n)->default_value(0), "Derivative order");
	options.add_options()(key("p"), boost::program_options::value(&p)->default_value(1), "Parameter of Minkowski norm");
	options.add_options()(key("sigma"), boost::program_options::value(&sigma)->default_value(0), "Standard deviation of Gaussian smoothing filter");

	options.add_options()(key("meta,M"), boost::program_options::value(&metaFiles)->multitoken(), "List of meta files");
	options.add_options()(key("load,l"), boost::program_options::value(&loadFile)->default_value(""), "Load illuminant estimator from file");
	options.add_options()(key("save,s"), boost::program_options::value(&saveFile)->default_value(""), "Save illuminant estimator to file");
}
#endif // WITH_BOOST

} // namespace illumestimators
