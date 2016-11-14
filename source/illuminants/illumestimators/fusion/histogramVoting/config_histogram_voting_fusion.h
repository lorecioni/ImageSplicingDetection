#ifndef CONFIG_HISTOGRAM_VOTING_FUSION_H
#define CONFIG_HISTOGRAM_VOTING_FUSION_H

#include "config.h"

namespace illumestimators {

class ConfigHistogramVotingFusion : public vole::Config
{
public:
	ConfigHistogramVotingFusion(const std::string& prefix = std::string());

//	ConfigHistogramVotingFusion& operator=( const vole::Config& other );
	ConfigHistogramVotingFusion& operator=( const ConfigHistogramVotingFusion& other );

	virtual bool parseOptionsDescription(const char* filename, boost::program_options::variables_map* vm = NULL);

	virtual std::string getString() const;

protected:
	virtual void initBoostOptions();

public:
	// options (should be public so classes that use this config can easily access them):
	double binsize;
	double lower_bound;
	double upper_bound;
	int window_size;
	bool force_relation;
//	int drawing_area_height;
//	int x_scale_multiplier;
};

} // namespace

#endif // CONFIG_HISTOGRAM_VOTING_FUSION_H
