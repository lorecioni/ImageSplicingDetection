#ifndef DB_INDEX_SHI_H
#define DB_INDEX_SHI_H

#include "shi_db_entry.h"
#include <string>
#include <vector>

namespace iread {

class DbIndexShi {
	public:
		DbIndexShi();
		DbIndexShi(std::string path, std::string index_file);

		std::vector<iread::ShiDbEntry> filelist;

	protected:

		void read_db();


		std::string path;
		std::string index_file;
		std::string full_file_name;
};

}
#endif // DB_INDEX_SHI_H
