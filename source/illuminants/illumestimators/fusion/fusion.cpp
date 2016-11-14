#include "fusion.h"

namespace illumestimators {

Fusion::Fusion(const std::vector<IlluminantEstimator*>& estimators) :
	m_estimators(estimators)
{

}

Fusion::~Fusion()
{

}

} // namespace illumestimators
