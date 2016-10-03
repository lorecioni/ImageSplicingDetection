#ifndef LILLE_COMMAND_COLOR_ILLUM_FIELD_H
#define LILLE_COMMAND_COLOR_ILLUM_FIELD_H

#include "command.h"
#include "config_iebv.h"
#include "illum.h"

namespace lille {

class CommandColorIllumField : public vole::Command
{
public:
	CommandColorIllumField();
	int execute();
	int execute(illumestimators::Illum &estimate);

	void printShortHelp() const;
	void printHelp() const;

	iic::ConfigIebv config;
protected:
	illumestimators::Illum estimate_;

private:


};


}

#endif // LILLE_COMMAND_COLOR_ILLUM_FIELD_H
