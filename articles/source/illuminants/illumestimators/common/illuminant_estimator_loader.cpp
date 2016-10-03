#include "illuminant_estimator_loader.h"
#include "baseline/meanestimator.h"
#include "baseline/nothingestimator.h"
#include "grayworld/grayworldestimator.h"
#include "bayesian/bayesianestimator.h"

namespace illumestimators {

IlluminantEstimator* IlluminantEstimatorLoader::fromFile(const std::string& filename)
{
	cv::FileStorage	fs(filename, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		return NULL;
	}

	std::string name = fs["name"];

	if (name == "MeanEstimator") {
		MeanEstimator* estimator = new MeanEstimator();
		if (estimator->load(filename)) {
			return estimator;
		} else {
			delete estimator;
			return NULL;
		}
	} else if (name == "NothingEstimator") {
		NothingEstimator* estimator = new NothingEstimator();
		if (estimator->load(filename)) {
			return estimator;
		} else {
			delete estimator;
			return NULL;
		}
	} else if (name == "GrayWorldEstimator") {
		GrayWorldEstimator* estimator = new GrayWorldEstimator();
		if (estimator->load(filename)) {
			return estimator;
		} else {
			delete estimator;
			return NULL;
		}
	} else if (name == "BayesianEstimator") {
		BayesianEstimator* estimator = new BayesianEstimator();
		if (estimator->load(filename)) {
			return estimator;
		} else {
			delete estimator;
			return NULL;
		}
	}
	return NULL;
}

} // namespace illumestimators
