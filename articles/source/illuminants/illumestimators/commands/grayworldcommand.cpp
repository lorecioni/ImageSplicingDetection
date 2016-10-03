#include "grayworldcommand.h"
#include "grayworld/grayworldestimator.h"
#include "metadata/metadatastorage.h"
#include "pipeline/single_illuminant_pipeline.h"
#include <opencv2/highgui/highgui.hpp>

namespace illumestimators {

GrayWorldCommand::GrayWorldCommand():
	Command("grayworld",
		config,
		"Michael Bleier",
		"michael.bleier@mb.stud.uni-erlangen.de"
	)
{
}

int GrayWorldCommand::execute()
{
	illumestimators::GrayWorldEstimator estimator(config.n, config.p, config.sigma);

	SingleIlluminantPipeline::loadIlluminantEstimator(estimator, config.loadFile);

	MetadataStorage storage(config.metaFiles);

	Illum estimate;
	double error;

	if (!SingleIlluminantPipeline::runEstimator(estimate, error, estimator, config.inputFile, storage, config.verbosity)) {
		return -1;
	}

	SingleIlluminantPipeline::saveIlluminantEstimator(estimator, config.saveFile);

	return 0;
}

void GrayWorldCommand::printShortHelp() const
{
	std::cout << "Generalized Gray World illuminant estimator." << std::endl;
}

void GrayWorldCommand::printHelp() const
{
	std::cout << "Generalized Gray World illuminant estimator." << std::endl;
}

} // namespace illumestimators
