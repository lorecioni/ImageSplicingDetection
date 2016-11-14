#ifndef LILLE_COMMAND_LGRAYWORLD_H
#define LILLE_COMMAND_LGRAYWORLD_H

#include "command.h"
#include "lgrayworld_config.h"

namespace lille {

class CommandLocalGrayworld : public vole::Command {
public:
	CommandLocalGrayworld();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LGrayWorldConfig config;
};

} // namespace lille

#endif // LILLE_COMMAND_LGRAYWORLD_H
