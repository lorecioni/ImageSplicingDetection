#ifndef ILLUMESTIMATORS_APPLY_ILLUM_ESTIMATORS_CONFIG
#define ILLUMESTIMATORS_APPLY_ILLUM_ESTIMATORS_CONFIG

#include "config.h"
#include <vector>

namespace illumestimators {

class ApplyEstimatorsConfig : public vole::Config {
public:
	ApplyEstimatorsConfig(const std::string& prefix = std::string());

public:
	virtual std::string getString() const;

protected:
	#ifdef WITH_BOOST
		virtual void initBoostOptions();
	#endif // WITH_BOOST

public:
	std::string inputFile;
	std::string output_path;

	bool gamut;
	bool bayes;
	bool white_patch_retinex;
	bool grayworld_vanilla;
	bool grayworld_first_order;
	bool grayworld_second_order;
	bool grayworld_best;

	std::vector<std::string> trainingFiles;
	int method;
	int n;
	double sigma;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_APPLY_ILLUM_ESTIMATORS_CONFIG
