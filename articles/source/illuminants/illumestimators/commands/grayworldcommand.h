#ifndef ILLUMESTIMATORS_COMMANDS_GRAYWORLDCOMMAND
#define ILLUMESTIMATORS_COMMANDS_GRAYWORLDCOMMAND

#include "command.h"
#include "grayworldconfig.h"

namespace illumestimators {

class GrayWorldCommand : public vole::Command {
public:
	GrayWorldCommand();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	GrayWorldConfig config;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMANDS_GRAYWORLDCOMMAND
