#ifndef LILLE_COMMAND_LIEBV_H
#define LILLE_COMMAND_LIEBV_H

#include "command.h"
#include "liebv_config.h"
#include <opencv2/ml/ml.hpp>

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
