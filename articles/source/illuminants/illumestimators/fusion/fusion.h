#ifndef ILLUMESTIMATORS_FUSION_FUSION_H
#define ILLUMESTIMATORS_FUSION_FUSION_H

#include "illuminantestimator.h"

namespace illumestimators {

class Fusion : public IlluminantEstimator
{
public:
	Fusion() { }
	Fusion(const std::vector<IlluminantEstimator*>& estimators);
	virtual ~Fusion();

public:
	void setEstimators(const std::vector<IlluminantEstimator*>& estimators) { m_estimators = estimators; }
	std::vector<IlluminantEstimator*> getEstimators() const { return m_estimators; }

private:
	std::vector<IlluminantEstimator*> m_estimators;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_FUSION_FUSION_H
