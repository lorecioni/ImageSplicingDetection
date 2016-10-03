#include "metadatastorage.h"
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace illumestimators {

MetadataStorage::MetadataStorage(const std::string& filename, int verbosity) :
	m_verbosity(verbosity)
{
	isValid = loadStorageFile(filename);
	if (!isValid) {
		std::cerr << "MetadataStorage::MetadataStorage(): tried to load " << filename << ", but failed." << std::endl;
	}
}

MetadataStorage::MetadataStorage(const std::vector<std::string>& filenames, int verbosity) :
	m_verbosity(verbosity)
{
	isValid = false;
	bool __valid = false;
	for (unsigned int i = 0; i < filenames.size(); i++) {
		__valid = loadStorageFile(filenames[i]);
		if (!__valid) std::cerr << "MetadataStorage::MetadataStorage(): tried to load " << filenames[i] << " from file list (entry no. " << i << "), but failed." << std::endl;
		isValid |= __valid;
	}
	if (!isValid)
		std::cerr << "MetadataStorage::MetadataStorage(): all files in the list failed, aborted." << std::endl;
}

MetadataStorage::~MetadataStorage()
{

}

std::string MetadataStorage::normalizePath(const std::string& path) const
{
	std::string normalizedPath;

	char* actualpath = realpath(path.c_str(), NULL);
	if (!actualpath) {
		std::cerr << "The path \"" << path << "\" cannot be resolved!" << std::endl;
		return std::string();
	}
	normalizedPath = actualpath;
	free(actualpath);

	return normalizedPath;
}

bool MetadataStorage::valid() const
{
	return isValid;
}

bool MetadataStorage::hasMetadata(const std::string& filename) const
{
	if (!isValid) return false;
	return m_meta.find(normalizePath(filename)) != m_meta.end();
}

Metadata MetadataStorage::getMetadata(const std::string& filename)
{
	if (!isValid) throw std::runtime_error("MetadataStorage::getMetadata: metadata is invalid, aborted.");

	std::string normalizedFilename = normalizePath(filename);

	if (hasMetadata(normalizedFilename)) {
		return m_meta[normalizedFilename];
	} else {
		return Metadata(normalizedFilename);
	}
}

bool MetadataStorage::loadStorageFile(const std::string& filename)
{
	if (m_verbosity > 0) {
		std::cout << "Trying to load metadata storage file \"" << filename << "\"!" << std::endl << std::endl;
	}

	boost::filesystem::path path(filename);
	std::string parentPath = normalizePath(path.parent_path().string());

	std::ifstream in(filename.c_str(), std::ifstream::in);

	if (!in.good()) {
		std::cerr << "MetadataStorage::loadStorageFile: Error opening metadata storage file \"" << filename << "\", aborted." << std::endl << std::endl;
		return false;
	}

	std::string line;
	std::getline(in, line);

	std::vector<std::string> labels = tokenizeLine(line);

	if (m_verbosity > 0) {
		std::cout << "labels: ";
		for (unsigned int i = 0; i < labels.size(); i++) {
			std::cout << labels[i] << " ";
		}
		std::cout << std::endl;
		std::cout << "parent path: " << parentPath << std::endl << std::endl;
	}

	m_filenames.push_back(filename);

	while (std::getline(in, line)) {
		if(!parseLine(line, labels, parentPath)) {
			std::cerr << "MetadataStorage::loadStorageFile: Error parsing metadata from file \"" << filename << "\", aborted." << std::endl << std::endl;
			return false;
		}
	}
	return true;
}

void MetadataStorage::reset()
{
	m_filenames.clear();
	m_meta.clear();
}

std::vector<std::string> MetadataStorage::tokenizeLine(const std::string& line) const
{
	std::vector<std::string> tokens;

	boost::tokenizer<boost::escaped_list_separator<char> > tk(line, boost::escaped_list_separator<char>('\\', ',', '\"'));

	for (boost::tokenizer<boost::escaped_list_separator<char> >::iterator it(tk.begin()); it != tk.end(); it++) {
		tokens.push_back(*it);
	}

	return tokens;
}

bool MetadataStorage::parseLine(const std::string& line, const std::vector<std::string>& labels, const std::string& parentPath)
{
	std::vector<std::string> tokens = tokenizeLine(line);

	if (tokens.size() != labels.size()) {
		return false;
	}

	Metadata m;

	if (m.parse(labels, tokens, parentPath)) {
		if (m_verbosity > 0) {
			m.print();
		}

		m_meta[normalizePath(m.filename())] = m;
	} else {
		return false;
	}

	return true;
}

} // namespace illumestimators
