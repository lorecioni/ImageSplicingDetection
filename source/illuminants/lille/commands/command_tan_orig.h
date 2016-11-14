#ifndef LILLE_COMMAND_TAN_ORIG_H
#define LILLE_COMMAND_TAN_ORIG_H

#include "command.h"
#include "liebv_config.h"
#include <opencv2/ml/ml.hpp>

namespace lille {

class CommandTanOrig : public vole::Command {
public:
	CommandTanOrig();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LIebvConfig config;

};

} // namespace lille

#endif // LILLE_COMMAND_TAN_ORIG_H
