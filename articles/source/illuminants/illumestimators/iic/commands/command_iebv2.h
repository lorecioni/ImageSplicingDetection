#ifndef IIC_COMMAND_IEBV_H
#define IIC_COMMAND_IEBV_H

#include "command.h"
#include "config_iebv.h"
#include "illum.h"

namespace iic {

class CommandIebv : public vole::Command
{
public:
	CommandIebv();
	int execute();
	int execute(illumestimators::Illum &estimate);

	void printShortHelp() const;
	void printHelp() const;

	ConfigIebv config;
protected:
	illumestimators::Illum estimate_;
private:


};


}

#endif // IIC_COMMAND_IEBV_H
