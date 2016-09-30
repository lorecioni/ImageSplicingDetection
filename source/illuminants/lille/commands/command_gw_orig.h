#ifndef LILLE_COMMAND_GRAYWORLD_ORIG_H
#define LILLE_COMMAND_GRAYWORLD_ORIG_H

#include "command.h"
#include "lgrayworld_config.h"

namespace lille {

class CommandGrayworldOrig : public vole::Command {
public:
	CommandGrayworldOrig();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LGrayWorldConfig config;
};

} // namespace lille

#endif // LILLE_COMMAND_GRAYWORLD_ORIG_H
