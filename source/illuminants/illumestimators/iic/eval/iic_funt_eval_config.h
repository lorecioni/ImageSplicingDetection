#ifndef IIC_FUNT_EVAL_CONFIG_H
#define IIC_FUNT_EVAL_CONFIG_H

#include "config.h"

#include "cv.h"

#include <iostream>
#include <vector>

/**
 * Configuration parameters for the illuminant estimation by voting, to be
 * included by all classes that need to access it.
 */

namespace iic {

class FuntEvalConfig : public vole::Config {
public:

	FuntEvalConfig();

	/// input file name
	std::string input_file;
	/// directory for all intermediate files
	std::string output_directory;
	// subcommand (if required for a command)
	std::string method;

	std::string root_dir;
	std::string funt_filelist;
	std::string ground_truth;
	std::string iebv_config_file;
	std::string iebv_method;
	std::string iebv_submethod;
	int n_estimates;

	int pix_per_set; // if a selection of the images in the set should be done

	virtual std::string getString() const;

protected:
		virtual void initBoostOptions();
};

}

#endif // IIC_FUNT_EVAL_CONFIG_H
