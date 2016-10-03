#include "config_histogram_voting_fusion.h"

using namespace boost::program_options;

namespace illumestimators {

ConfigHistogramVotingFusion::ConfigHistogramVotingFusion( const std::string &prefix )
	: Config( prefix )
{
	// TODO: interestingly, this constructor is called 4 times when we run "vole iebv2 ...";
	// Why and is that okay? (Hence, also initBoostOptions() is called 4 times...)
//	std::cout << "STOP: ConfigHistogramVotingFusion::ConfigHistogramVotingFusion( const std::string &prefix )" << std::endl;
	initBoostOptions();
}

//ConfigHistogramVotingFusion& ConfigHistogramVotingFusion::operator=( const vole::Config& other )
//{
//	std::cout << "STOP: ConfigHistogramVotingFusion::operator=( const vole::Config& other )" << std::endl;
//	verbosity = other.verbosity;
//	prefix = other.prefix;
//	prefix_enabled = other.prefix_enabled;
//}

ConfigHistogramVotingFusion& ConfigHistogramVotingFusion::operator=( const ConfigHistogramVotingFusion& other )
{
	// inherited variables:
	verbosity = other.verbosity;
	prefix = other.prefix;
	prefix_enabled = other.prefix_enabled;
	// own variables:
	binsize = other.binsize;
	lower_bound = other.lower_bound;
	upper_bound = other.upper_bound;
	window_size = other.window_size;
	force_relation = other.force_relation;
//	drawing_area_height = other.drawing_area_height;
//	x_scale_multiplier = other.x_scale_multiplier;
	return *this;
}

bool ConfigHistogramVotingFusion::parseOptionsDescription( const char *filename, boost::program_options::variables_map *vm )
{
	std::cout << "STOP: ConfigHistogramVotingFusion::parseOptionsDescription( const char *filename, boost::program_options::variables_map *vm )" << std::endl;
	std::cout << "I THINK THIS SHOULD NEVER BE CALLED! AM I WRONG?" << std::endl;
	exit(1);
	return false;
}

std::string ConfigHistogramVotingFusion::getString() const
{
	std::stringstream s;
	if( prefix_enabled ) {
		s << "[" << prefix << "]" << std::endl;
	}

	s << "binsize=" << binsize
	  << " # size of a bin for creating the histogram from continous values" << std::endl;
	s << "lower_bound=" << lower_bound
	  << " # lower boundary for the histogram" << std::endl;
	s << "upper_bound=" << upper_bound
	  << " # upper boundary for the histogram" << std::endl;
	s << "window_size=" << window_size
	  << " # size of the mean-smoothing window" << std::endl;
	s << "force_relation=" << force_relation
	  << " # use if HistogramVoting should only accept combinations of R, G, and B estimates that add-up to 1." << std::endl;
//	s << "drawing_area_height=" << drawing_area_height
//	  << " # height of the drawing area" << std::endl;
//	s << "x_scale_multiplier=" << x_scale_multiplier
//	  << " # a multiplier to scale the x-axis" << std::endl;

	return s.str();
}

void ConfigHistogramVotingFusion::initBoostOptions()
{
	options.add_options()
			( key( "binsize" ), value( &binsize )->default_value( 0.01 ),
			  "size of a bin for creating the histogram from continous values" )
			( key( "lower_bound" ), value( &lower_bound )->default_value( 0 ),
			  "lower boundary for the histogram" )
			( key( "upper_bound" ), value( &upper_bound )->default_value( 1 ),
			  "upper boundary for the histogram" )
			// TODO: a window_size default value of 1 in the old code. As later, it will be used as "half_size"... (see HistogramVotingFusion class). Not intuitive?
			( key( "window_size" ), value( &window_size )->default_value( 1 ),
			  "size of the mean-smoothing window" )
			( key( "force_relation" ), bool_switch( &force_relation )->default_value( false ),
			  "use if HistogramVoting should only accept combinations of R, G, and B estimates"
			  " that add-up to 1." )
//		( key( "drawing_area_height"), value( &drawing_area_height)->default_value( 400 ),
//				"height of the drawing area" )
//		( key( "x_scale_multiplier"), value( &x_scale_multiplier )->default_value( 3 ),
//				"a multiplier to scale the x-axis" )
		;
}

} // namespace
