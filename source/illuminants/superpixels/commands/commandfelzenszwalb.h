/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_COMMANDS_COMMANDFELZENSZWALB
#define SUPERPIXELS_COMMANDS_COMMANDFELZENSZWALB

#include "command.h"
#include "felzenszwalbconfig.h"

namespace vole {

class CommandFelzenszwalb : public Command {
public:
	CommandFelzenszwalb();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	FelzenszwalbConfig config;
};

} // vole

#endif // SUPERPIXELS_COMMANDS_COMMANDFELZENSZWALB
