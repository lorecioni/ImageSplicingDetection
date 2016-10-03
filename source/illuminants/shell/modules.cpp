/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#include "modules.h"

namespace vole {

Modules::Modules() {
	Command *c;

	
	/* Algorithms for Superpixel Segmentation */
	c = new vole::CommandFelzenszwalb(); insert(std::make_pair(c->getName(), c));
	/* Local Illuminant Estimation */
	c = new lille::CommandLocalIebv(); insert(std::make_pair(c->getName(), c));
	/* Local Illuminant Estimation */
	c = new lille::CommandLocalGrayworld(); insert(std::make_pair(c->getName(), c));

}

}
