#ifndef ILLUMINANTESTIMATORS_PIPELINE_SINGLE_ILLUMINANT_PIPELINE_H
#define ILLUMINANTESTIMATORS_PIPELINE_SINGLE_ILLUMINANT_PIPELINE_H

#include "illuminantestimator.h"
#include "metadata/metadatastorage.h"

namespace illumestimators {

class SingleIlluminantPipeline {
private:
	SingleIlluminantPipeline() {}

public:
	static bool loadIlluminantEstimator(IlluminantEstimator& estimator, const std::string& filename);
	static bool saveIlluminantEstimator(const IlluminantEstimator& estimator, const std::string& filename);

public:
	static bool runTraining(IlluminantEstimator& estimator, const std::vector<std::string>& filenames, MetadataStorage& storage, int verbosity = 0);

public:
	static bool runEstimator(Illum& estimate, double& error, const IlluminantEstimator& estimator, const std::string& filename, MetadataStorage& storage, int verbosity = 0);
};

} // illumestimators

#endif // ILLUMINANTESTIMATORS_PIPELINE_SINGLE_ILLUMINANT_PIPELINE_H
