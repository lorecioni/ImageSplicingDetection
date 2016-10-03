#include <fstream>

#include <boost/version.hpp>

#if BOOST_VERSION >= 104400
	#define VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 1

	#ifndef BOOST_FILESYSTEM_VERSION
	#define BOOST_FILESYSTEM_VERSION 3
	#endif 
	#include <boost/filesystem.hpp>
	using namespace boost::filesystem;
#endif

#include "get_file.h"


namespace vole { namespace GetFile {

	bool create_path(std::string directory)
	{
		#ifndef VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 
			return false;
		#else
			path p(directory);
			if (exists(p)) return (is_directory(p) ? true : false);

			// non-existing: try to create directories recursively, fatal failure
			// if one of these directories can not be created.
//			boost::system::error_code ec;
			return create_directories(p); //, ec);
		#endif // VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 
	}

	bool create_file_and_path(std::string file)
	{
		#ifndef VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 
			return false;
		#else // VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 
			path p(file);
			if (exists(p)) return (is_directory(p) ? false : true);

			if (p.has_parent_path() && !create_path(p.parent_path().generic_string())) return false;

			// test-open the file for writing;
			std::ofstream testMe(file.c_str(), std::ios::app);
			if (!testMe.good()) return false;
			testMe.close();
			return true;
		#endif // VOLE_GET_FILE_IMPLEMENT_FILESYSTEM 
	}

} } // end namespace vole::GetFile
