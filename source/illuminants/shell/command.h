/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <iostream>
#include <boost/program_options.hpp>
#include "config.h"

namespace vole {


class Command {

public:
	Command(const std::string &name, Config& config,
	        const std::string &contributor_name = "",
	        const std::string &contributor_mail = "")
	 : name(name), contributor_name(contributor_name),
	   contributor_mail(contributor_mail), abstract_config(config)  {}

	virtual ~Command() {}
	virtual const std::string& getName() const { return name; }
	virtual const std::string& getContributorName() const { return contributor_name; }
	virtual const std::string& getContributorMail() const { return contributor_mail; }
	virtual Config& getConfig() { return abstract_config; }
	virtual int execute() = 0;
	virtual void printShortHelp() const = 0;
	virtual void printHelp() const = 0;

protected:
	std::string name;
	std::string contributor_name;
	std::string contributor_mail;

private:
	Config& abstract_config;

};

}

#endif // COMMAND_H
