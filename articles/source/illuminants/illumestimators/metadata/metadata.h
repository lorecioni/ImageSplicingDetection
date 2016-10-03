#ifndef ILLUMESTIMATORS_METADATA_METADATA_H
#define ILLUMESTIMATORS_METADATA_METADATA_H

#include "illum.h"
#include <opencv2/core/core.hpp>

namespace illumestimators {

class Metadata
{
public:
	Metadata();
	Metadata(const std::string& filename);
	~Metadata();

public:
	std::string filename() const { return m_filename; }
	std::string database() const { return m_database; }
	std::string scene() const { return m_scene; }
	std::string colorspace() const { return m_colorspace; }
	std::string lightSources() const { return m_lightSources; }
	Illum dominantIlluminant() const { return m_dominantIlluminant; }
	std::string illuminantMapFilename() const { return m_illuminantMapFilename; }
	cv::Mat_<cv::Vec3d> illuminantMap() const;
	std::string maskFilename() const { return m_maskFilename; }
	cv::Mat_<unsigned char> mask() const;
	std::string targetFilename() const { return m_targetFilename; }
	cv::Mat_<unsigned char> target() const;

public:
	void print() const;
	bool parse(std::vector<std::string> labels, std::vector<std::string> values, const std::string& parentPath);

private:
	std::string fixPath(const std::string& path, const std::string& parentPath) const;

private:
	/// full filename of the image
	std::string m_filename;
	/// name of the database
	std::string m_database;
	/// name of the scene
	std::string m_scene;
	/// colorspace of the image (e.g. srgb, rgb)
	std::string m_colorspace;
	/// lightsources used during capturing (e.g. wr)
	std::string m_lightSources;
	/// chromaticities of the dominant light source
	Illum m_dominantIlluminant;
	/// full filename of image with local illuminant chromaticities
	std::string m_illuminantMapFilename;
	/// full filename of mask with unusable image regions
	std::string m_maskFilename;
	/// full filename of mask with reflection target(s)
	std::string m_targetFilename;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_METADATA_METADATA_H
