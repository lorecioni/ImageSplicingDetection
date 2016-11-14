#ifndef ILLUMESTIMATORS_COMMON_ILLUMINANT_ESTIMATOR_LOADER_H
#define ILLUMESTIMATORS_COMMON_ILLUMINANT_ESTIMATOR_LOADER_H

#include "illuminantestimator.h"

namespace illumestimators {

class IlluminantEstimatorLoader
{
public:
	static IlluminantEstimator* fromFile(const std::string& filename);
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMON_ILLUMINANT_ESTIMATOR_LOADER_H
