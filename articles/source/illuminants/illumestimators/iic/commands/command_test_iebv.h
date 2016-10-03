#ifndef IIC_COMMAND_TEST_IEBV_H
#define IIC_COMMAND_TEST_IEBV_H

#include "command.h"
#include "config_iebv.h"

namespace iic {

class CommandTestIebv : public vole::Command {
public:
	CommandTestIebv();
	int execute();
	void printShortHelp() const;
	void printHelp() const;

protected:
	ConfigIebv config;

};

}

#endif // IIC_COMMAND_TEST_IEBV_H
