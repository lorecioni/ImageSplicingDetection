#ifndef SUPERPIXELS_COMMANDS_COMMANDVEKSLER
#define SUPERPIXELS_COMMANDS_COMMANDVEKSLER

#include "command.h"
#include "vekslerconfig.h"

namespace vole {

class CommandVeksler : public Command {
public:
	CommandVeksler();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	VekslerConfig config;
};

} // vole

#endif // SUPERPIXELS_COMMANDS_COMMANDVEKSLER
