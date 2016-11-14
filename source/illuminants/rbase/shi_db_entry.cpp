#include "shi_db_entry.h"
#include <sstream>

namespace iread {

	ShiDbEntry::ShiDbEntry()
	{
	}

	void ShiDbEntry::set_dark_and_saturation_level()
	{
		// SHOULD BE WRITTEN IN THE DATABASE, BUT NEVER MIND
		if (image_file_short.find("IMG_") != std::string::npos) { // subtract dark level
			dark_level = 129;
			saturation_level = 3690;
		} else {
			dark_level = 0;
			saturation_level = 3690;
		}
	}

	void ShiDbEntry::expand_paths(std::string path)
	{
		this->path = path;
		{
			std::stringstream s;
			s << path << "/" << image_file_short;
			image_file = s.str();
		}
		{
			std::stringstream s;
			s << path << "/" << mask_file_short;
			mask_file = s.str();
		}
		{
			std::stringstream s;
			s << path << "/" << target_file_short;
			target_file = s.str();
		}
	}
}

