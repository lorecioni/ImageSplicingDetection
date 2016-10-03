#ifndef LILLE_COMMAND_LTAN_H
#define LILLE_COMMAND_LTAN_H

#include "command.h"
#include "liebv_config.h"
#include <opencv2/ml/ml.hpp>

namespace lille {

class CommandLocalTan : public vole::Command {
public:
	CommandLocalTan();

public:
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	LIebvConfig config;

};

} // namespace lille

#endif // LILLE_COMMAND_LTAN_H
