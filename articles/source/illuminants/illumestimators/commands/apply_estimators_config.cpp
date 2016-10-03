#include "apply_estimators_config.h"

using namespace boost::program_options;

namespace illumestimators {

ApplyEstimatorsConfig::ApplyEstimatorsConfig(const std::string& prefix) :
	Config(prefix)
{
	#ifdef WITH_BOOST
		initBoostOptions();
	#endif // WITH_BOOST
}

std::string ApplyEstimatorsConfig::getString() const
{
	std::stringstream s;

	if (prefix_enabled) {
		s << "[" << prefix << "]" << std::endl;
	} else {
		s	<< "input=" << inputFile << " # Image to process" << std::endl
			<< "output=" << output_path << " # Working directory" << std::endl;
	}
	s	<< "bayes=" << (bayes ? 1 : 0) << " # perform bayes evaluation" << std::endl
		<< "gamut=" << (gamut ? 1 : 0) << " # perform gamut 2d evaluation" << std::endl
		<< "retinex=" << (white_patch_retinex ? 1 : 0) << " # perform white patch retinex" << std::endl
		<< "gw0=" << (grayworld_vanilla ? 1 : 0) << " # perform vanilla gray world" << std::endl
		<< "gw1=" << (grayworld_first_order ? 1 : 0) << " # perform 1st order gray world" << std::endl
		<< "gw2=" << (grayworld_second_order ? 1 : 0) << " # perform 2nd order gray world" << std::endl
		<< "gwbest=" << (grayworld_best ? 1 : 0) << " # look for best gray world" << std::endl
	;

	return s.str();
}

#ifdef WITH_BOOST
void ApplyEstimatorsConfig::initBoostOptions()
{
	if (!prefix_enabled) {
		options.add_options()
			(key("input,I"), value(&inputFile)->default_value(""), "Image to process")
			(key("output,O"), value(&output_path)->default_value(""), "Working directory")

		;
	}

	options.add_options()
		(key("bayes,B"), bool_switch(&bayes)->default_value(false), "perform bayes evaluation")
		(key("gamut,G"), bool_switch(&gamut)->default_value(false), "perform gamut 2d evaluation")
		(key("retinex,R"), bool_switch(&white_patch_retinex)->default_value(false), "perform white patch retinex")
		(key("gw0,W"), bool_switch(&grayworld_vanilla)->default_value(false), "perform vanilla gray world")
		(key("gw1,X"), bool_switch(&grayworld_first_order)->default_value(false), "perform 1st order gray world")
		(key("gw2,Y"), bool_switch(&grayworld_second_order)->default_value(false), "perform 2nd order gray world")
		(key("gwbest,Z"), bool_switch(&grayworld_best)->default_value(false), "look for best gray world")
	;
}
#endif // WITH_BOOST

} // namespace illumestimators
