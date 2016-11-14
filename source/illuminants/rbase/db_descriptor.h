#ifndef RBASE_DB_DESCRIPTOR_H
#define RBASE_DB_DESCRIPTOR_H

#include <string>

namespace rbase {

class DbDescriptor {

	public:
		DbDescriptor(int verbosity = 0);
		DbDescriptor(std::string file, int verbosity = 0);

		virtual ~DbDescriptor();

		bool init(std::string file);

		bool isValid;

		int nScenes;

		int *picturesPerScene;  // number of entries: nScenes (one for each scene)
		int *dark_level;        // number of entries: nScenes (one for each scene)
		int *saturation_level; // number of entries: nScenes (one for each scene)
		std::string *scene; // number of entries: nScenes (one for each scene)
		std::string *segmentation; // number of entries: nScenes (one for each scene)
		std::string *mask;         // number of entries: nScenes (one for each scene)

		std::string **gtFiles;   // number of entries: (nScenes, picturesPerScene[sceneNo]) 
		std::string **normFiles; // number of entries: (nScenes, picturesPerScene[sceneNo]) 

	protected:
		bool readFile(std::string file);
		void cleanUp();

		int verbosity;
		std::string fileName;
};

}

#endif // RBASE_DB_DESCRIPTOR_H
