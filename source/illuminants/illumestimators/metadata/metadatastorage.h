#ifndef ILLUMESTIMATORS_METADATA_METADATA_STORAGE_H
#define ILLUMESTIMATORS_METADATA_METADATA_STORAGE_H

#include "metadata.h"

namespace illumestimators {

class MetadataStorage
{
public:
	MetadataStorage(const std::string& filename = std::string(), int verbosity = 0);
	MetadataStorage(const std::vector<std::string>& filenames, int verbosity = 0);
	~MetadataStorage();

public:
	bool valid() const;
	bool hasMetadata(const std::string& filename) const;
	Metadata getMetadata(const std::string& filename);

public:
	bool loadStorageFile(const std::string& filename);

public:
	void reset();

private:
	std::vector<std::string> tokenizeLine(const std::string& line) const;
	bool parseLine(const std::string& line, const std::vector<std::string>& columnHeaders, const std::string& parentPath);
	std::string normalizePath(const std::string& path) const;

private:
	int m_verbosity;
	std::vector<std::string> m_filenames;
	std::map<std::string, Metadata> m_meta;
	bool isValid;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_METADATA_METADATA_STORAGE_H
