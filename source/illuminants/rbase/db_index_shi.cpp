#include "db_index_shi.h"

#include "string_conversion.h" // from vole/core/common/
#include <fstream>
#include <iostream>



namespace iread {

DbIndexShi::DbIndexShi()
	: path("/net/cv/illum/shi"), index_file("gehler_raw12bit.csv")
{
	full_file_name = path + "/" + index_file;
	read_db();
}

DbIndexShi::DbIndexShi(std::string path, std::string index_file)
	: path(path), index_file(index_file)
{
	full_file_name = path + "/" + index_file;
	read_db();
}

void DbIndexShi::read_db() {

	std::ifstream in(full_file_name.c_str(), std::ifstream::in);

	std::string line;
	std::getline(in, line);

	if (in.good()) filelist.clear();

	while (in.good()) {
		std::string line;
		std::getline(in, line);
		// file content looks e.g. like this:
		//filename,database,scene,colorspace,r,g,b,mask,target
		//raw12bit/8D5U5524.png,gehler,indoor,rgb,0.311967,0.423123,0.264908,raw12bit_mask/8D5U5524_mask.pbm,raw12bit_target/8D5U5524_target.pbm

		std::vector<std::string> tokens = vole::StringConversion::tokenizeLine(line, ',');
		if (tokens.size() < 8) continue;

		ShiDbEntry entry;
		entry.image_file_short  = tokens[0];
		entry.database          = tokens[1];
		entry.scene             = tokens[2];
		entry.colorspace        = tokens[3];
		// gt: 4, 5, 6
		entry.mask_file_short   = tokens[7];
		entry.target_file_short = tokens[8];

		double r, g, b;
		{ std::istringstream iss(tokens[4]); iss >> r; }
		{ std::istringstream iss(tokens[5]); iss >> g; }
		{ std::istringstream iss(tokens[6]); iss >> b; }
		entry.gt = rbase::Color(r, g, b);

		entry.expand_paths(path);
		entry.set_dark_and_saturation_level();
		filelist.push_back(entry);
	}
}


}

