#ifndef IIC_COMMAND_TEST_CACHE_H
#define IIC_COMMAND_TEST_CACHE_H

#include "command.h"
#include "config_iebv.h"

namespace iic {

class CommandTestCache : public vole::Command
{
public:
	CommandTestCache();
	int execute();

	void printShortHelp() const;
	void printHelp() const;

	ConfigIebv config;
protected:

private:


};


}

#endif // IIC_COMMAND_TEST_CACHE_H
