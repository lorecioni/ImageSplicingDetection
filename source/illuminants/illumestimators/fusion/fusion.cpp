/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

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
