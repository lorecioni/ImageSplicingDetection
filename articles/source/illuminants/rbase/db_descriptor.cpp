#include "db_descriptor.h"

#include <iostream>
#include <fstream>

namespace rbase {

DbDescriptor::DbDescriptor(int verbosity)
	: isValid(false), verbosity(verbosity)
{
	nScenes = 0;
	picturesPerScene = NULL;
	dark_level = NULL;
	saturation_level = NULL;
	scene = NULL;
	segmentation = NULL;
	mask = NULL;

	gtFiles = NULL;
	normFiles = NULL;
	fileName = "";
}

DbDescriptor::DbDescriptor(std::string file, int verbosity)
	: verbosity(verbosity)
{
	nScenes = 0;
	picturesPerScene = NULL;
	dark_level = NULL;
	saturation_level = NULL;
	scene = NULL;
	segmentation = NULL;
	mask = NULL;

	gtFiles = NULL;
	normFiles = NULL;
	fileName = file;

	init(fileName);
}

void DbDescriptor::cleanUp()
{
	if (nScenes > 0) {
		if (gtFiles != NULL) {
			for (int i = 0; i < nScenes; ++i)
				if (gtFiles[i] != NULL)
					delete[] gtFiles[i];
		}
		if (normFiles != NULL) {
			for (int i = 0; i < nScenes; ++i)
				if (normFiles[i] != NULL)
					delete[] normFiles[i];
		}
	}
	if (gtFiles != NULL) delete[] gtFiles;
	if (normFiles != NULL) delete[] normFiles;
	if (picturesPerScene != NULL) delete[] picturesPerScene;
	if (dark_level != NULL) delete[] dark_level;
	if (saturation_level != NULL) delete[] saturation_level;
	if (scene != NULL) delete[] scene;
	if (segmentation != NULL) delete[] segmentation;
	if (mask != NULL) delete[] mask;
	nScenes = 0;
	isValid = false;
	fileName = "";
}

DbDescriptor::~DbDescriptor()
{
	cleanUp();
}

bool DbDescriptor::init(std::string file)
{
	if (isValid)
		cleanUp();

	return readFile(file);
}


bool DbDescriptor::readFile(std::string file)
{
	isValid = false;
	this->fileName = file;

	std::ifstream in;
	in.open(file.c_str());
	if (!in.good()) {
		if (verbosity > 0) std::cerr << "unable to open list file " << file << std::endl;
		return false; 
	}

	in >> nScenes;

	picturesPerScene = new int[nScenes];
	dark_level = new int[nScenes];
	saturation_level = new int[nScenes];
	scene = new std::string[nScenes]; // 4 in the case of the Michael database, 11 for shida
	segmentation = new std::string[nScenes]; // 4 in the case of the Michael database, 11 for shida
	mask = new std::string[nScenes]; // 4 in the case of the Michael database, 11 for shida
	for (int i = 0; i < nScenes; ++i) {
		in >> scene[i];
		in >> segmentation[i];
		in >> mask[i];
	}
	
	gtFiles = new std::string*[nScenes];
	normFiles = new std::string*[nScenes];

	std::string key;
	for (int i = 0; i < nScenes; ++i) {
		in >> picturesPerScene[i] >> dark_level[i] >> saturation_level[i];
		gtFiles[i] = new std::string[picturesPerScene[i]];
		normFiles[i] = new std::string[picturesPerScene[i]];

		for (int j = 0; j < picturesPerScene[i]; ++j) {
			in >> key >> gtFiles[i][j] >> normFiles[i][j];
		}
	}

	in.close();
	isValid = true;
	return true;
}

}
