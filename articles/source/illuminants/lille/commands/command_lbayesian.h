#ifndef LILLE_COMMAND_LBAYESIAN_H
#define LILLE_COMMAND_LBAYESIAN_H

#include "command.h"
#include "lbayesian_config.h"

namespace lille {

class CommandLocalBayesian : public vole::Command {
public:
	CommandLocalBayesian();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LBayesianConfig config;
};

} // namespace lille

#endif // LILLE_COMMAND_LGRAYWORLD_H
