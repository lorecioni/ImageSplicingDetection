/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_COMMANDS_PATCHESCONFIG
#define SUPERPIXELS_COMMANDS_PATCHESCONFIG

#include "config.h"

namespace vole {

class PatchesConfig : public Config {
public:
	PatchesConfig( const std::string& prefix = std::string() );

public:
	virtual std::string getString() const;

protected:
	#ifdef WITH_BOOST
		virtual void initBoostOptions();
	#endif	// WITH_BOOST

public:
	int size_x;
	int size_y;
	int number_tries;
	std::string strategy;
};

} // namespace vole

#endif // SUPERPIXELS_COMMANDS_PATCHESCONFIG
