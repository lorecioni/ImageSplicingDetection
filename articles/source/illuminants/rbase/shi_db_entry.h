#ifndef IREAD_SHI_DB_ENTRY_H
#define IREAD_SHI_DB_ENTRY_H

#include "color.h"
#include <string>

namespace iread {

class ShiDbEntry {
	public:
		ShiDbEntry();

		void expand_paths(std::string path);
		void set_dark_and_saturation_level();

		std::string database;
		std::string scene;
		std::string colorspace;
		rbase::Color gt; // r,g,b
		int dark_level;
		int saturation_level;

		std::string image_file_short;
		std::string mask_file_short;
		std::string target_file_short;

		std::string path; // root directory of the database

		std::string image_file; // full filename + path to the image
		std::string mask_file; // full filename + path to the mask
		std::string target_file; // full filename + path to the target
};

}
#endif // IREAD_SHI_DB_ENTRY_H
