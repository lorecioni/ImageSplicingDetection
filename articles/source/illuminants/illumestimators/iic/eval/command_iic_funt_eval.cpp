#include <iostream>
#include <fstream>

#include "command_iic_funt_eval.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "boost/version.hpp"
#if BOOST_VERSION >= 104400
	#ifndef BOOST_FILESYSTEM_VERSION 
	#define BOOST_FILESYSTEM_VERSION 3
	#endif
#else
	#error At least boost version 1.44.0 required (boost::filesystem version 3) - current version is older (BOOST_VERSION), aborted.
	#ifndef BOOST_FILESYSTEM_VERSION 
	#define BOOST_FILESYSTEM_VERSION 2 // careful, unique_path will break with older boost version
	#endif
#endif    
#include "boost/filesystem.hpp"

#ifdef WIN32 // Visual Studio fuckup; if anybody knows how to do it better, I'd be curious to know
#define random() rand()
#define srandom(a) srand(a)
#endif

#include "command_iebv2.h"

using namespace boost::program_options;
using namespace boost::filesystem;

// if we want an enum for configuration, we need to call this
// ENUM_MAGIC(boundcheck);

namespace iic {

FuntEval::FuntEval()
 : vole::Command(
		"iic_funt",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de")
{
}

FuntEval::~FuntEval() {
}


int FuntEval::execute() {
	if (config.verbosity > 0) std::cout << "Evaluation: IIC-based methods on Ciurea/Funt" << std::endl;

	// read file list
	std::ifstream file_list;
	file_list.open(config.funt_filelist.c_str());
	if (!file_list.is_open()) {
		std::cerr << "ERROR: Can not open " << config.funt_filelist << " , aborted" << std::endl;
		return 1;	
	}

	if (config.pix_per_set > 0) {
		// create hash map of all sets and the number of images;
		std::map<std::string, std::vector<std::string> > sets;
		std::string cur_img;
		while(file_list >> cur_img) {
			// find last file separator ("/") in file name -> directory path = set name
			size_t cut_pos = cur_img.find_last_of('/');
			std::string path = cur_img.substr(0, cut_pos);
			if (sets.find(path) == sets.end()) {
				std::vector<std::string> tmp;
				sets.insert(std::pair<std::string, std::vector<std::string> >(path, tmp));
			}
			sets[path].push_back(cur_img);
		}
		file_list.close();
//		std::stringstream s;
//		srandom(time(0));
//		s << "vl" << random() % 1000; 
		std::string tmpFilename = boost::filesystem::unique_path().string() + ".txt"; //   = tempnam("/tmp/", s.str().c_str());
		std::ofstream tmp;
		tmp.open(tmpFilename.c_str());
		if (!tmp.is_open()) {
			std::cerr << "ERROR: Can not open temporary file " << tmpFilename
				<< " , aborted" << std::endl;
			return 1;	
		}

		std::map<std::string, std::vector<std::string> >::iterator it;
		for (it = sets.begin(); it != sets.end(); it++) {
			std::vector<std::string> &cur = it->second;
			if (config.pix_per_set == 1) {
				tmp << cur[0] << std::endl;
			} else {
				int step_size = static_cast<int>((cur.size()-1) / (config.pix_per_set-1));
				if (step_size < 1) step_size = 1;
				int upperLimit = std::min<int>(config.pix_per_set, cur.size());
				for (int i = 0; i < upperLimit; ++i) {
					tmp << cur[i*step_size] << std::endl;
				}
			}
		}
		tmp.close();
		file_list.open(tmpFilename.c_str());
		if (!file_list.is_open()) {
			std::cerr << "ERROR: Can not re-open temporary file " << tmpFilename
				<< " , aborted" << std::endl;
			return 1;	
		}
	}

	// create output directory
	if (config.output_directory.length() < 1) {
		std::cerr << "ERROR: No output directory specified, aborted." << std::endl;
		return 1;	
	}

	path output_path(config.output_directory);
	if (!exists(output_path)) {
		create_directory(output_path);
		std::cout << output_path << " created" << std::endl;
	}

	// create iebv_config
//	TODO GANZ ANDERS MACHEN
//	CommandIEbV illum;

	CommandIebv illum;
	// adjust parameters here

	// dump iebv config in this directory
	bool success = illum.config.parseOptionsDescription(config.iebv_config_file.c_str());
	if (!success) {
		std::cerr << "ERROR: Unable to parse iebv config file " << config.iebv_config_file << ", aborted" << std::endl;
		return 1;
	}
   // illum.config.seg_patches.strategy = config.iebv_submethod;

	std::ofstream config_out( (output_path / "iebv_config.txt").string().c_str() );
	if (!config_out.is_open()) {
		std::cerr << "ERROR: Can not open iebv configuration output file " << config_out
			<< " , aborted" << std::endl;
		return 1;	
	}
	config_out << illum.config.getString();
	config_out.close();

	// open result file
	std::string results_file = (output_path / "eval_result.txt").string();
	std::ofstream results_out(results_file.c_str());
	if (!results_out.is_open()) {
		std::cerr << "ERROR: Can not open results_file " << results_file << ", aborted" <<std::endl;
		return 1;
	}


	// process funt file list
	std::string cur_img;
	while (file_list >> cur_img) {
		if (config.verbosity > 0) std::cout << "current image: " << cur_img << std::endl;
		// put full file name together
		
		// estimate every image n times;
		for (int i = 0; i < config.n_estimates; ++i) {
			// create instance of iebv
			{
				//	TODO GANZ ANDERS MACHEN
				//	CommandIEbV illum;
				CommandIebv cur_illum;
				// TODO set further options
				bool success = cur_illum.config.parseOptionsDescription(config.iebv_config_file.c_str());
				if (!success) {
					std::cerr << "ERROR: Unable to parse iebv config file " << config.iebv_config_file << ", aborted" << std::endl;
					return 1;
				}
				cur_illum.config.input_file = std::string(config.root_dir + "/" + cur_img);
//				cur_illum.config.seg_patches.strategy = config.iebv_submethod;
//				cur_illum.config.verbosity = 0;
				
				illumestimators::Illum estimate;

				cur_illum.execute(estimate);
				results_out << cur_img << " " << estimate.r << " " << estimate.g << " " << estimate.b << " " << std::endl;
			}
		}
	}
	
	file_list.close();
	return 0;
}



void FuntEval::printShortHelp() const {
	std::cout << "Evaluation of IIC-based methods on the Ciurea/Funt database" << std::endl;
}


void FuntEval::printHelp() const {
	std::cout << "Evaluation of IIC-based methods on the Ciurea/Funt database" << std::endl;
	std::cout << std::endl;
	std::cout << "please read the technical report (and/or extend this help)" << std::endl << std::endl;
}

}
