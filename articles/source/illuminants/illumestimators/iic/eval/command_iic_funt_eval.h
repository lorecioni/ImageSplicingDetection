#ifndef COMMAND_IIC_FUNT_EVAL_H
#define COMMAND_IIC_FUNT_EVAL_H

#include <boost/program_options.hpp>
#include <iostream>

#include "command.h"
#include "iic_funt_eval_config.h"


using namespace boost::program_options;

namespace iic {

class FuntEval : public vole::Command {
public:
	FuntEval();
	~FuntEval();
	int execute();

	void printShortHelp() const;
	void printHelp() const;

	FuntEvalConfig config;

protected:

private:
};

}

#endif // COMMAND_IIC_FUNT_EVAL_H
