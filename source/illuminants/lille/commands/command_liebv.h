/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef LILLE_COMMAND_LIEBV_H
#define LILLE_COMMAND_LIEBV_H

#include "command.h"
#include "liebv_config.h"

namespace lille {

class CommandLocalIebv : public vole::Command {
public:
	CommandLocalIebv();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LIebvConfig config;

};

} // namespace lille

#endif // LILLE_COMMAND_LIEBV_H
