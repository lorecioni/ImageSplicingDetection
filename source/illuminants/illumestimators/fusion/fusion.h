/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

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
