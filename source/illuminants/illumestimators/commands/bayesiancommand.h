#ifndef ILLUMESTIMATORS_COMMANDS_BAYESIANCOMMAND
#define ILLUMESTIMATORS_COMMANDS_BAYESIANCOMMAND

#include "command.h"
#include "bayesianconfig.h"

namespace illumestimators {

class BayesianCommand : public vole::Command {
public:
	BayesianCommand();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	BayesianConfig config;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_COMMANDS_GRAYWORLDCOMMAND
