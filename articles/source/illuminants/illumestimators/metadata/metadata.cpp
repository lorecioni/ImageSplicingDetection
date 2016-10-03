#include "metadata.h"
#include <opencv2/highgui/highgui.hpp>

#include <boost/version.hpp>

#if BOOST_VERSION >= 104500
	#ifndef BOOST_FILESYSTEM_VERSION
    	#define BOOST_FILESYSTEM_VERSION 3
	#endif
#else
	#ifndef BOOST_FILESYSTEM_VERSION
		#define BOOST_FILESYSTEM_VERSION 2
	#endif
	#error detected boost version is lower than 1.45, I think I will die.
#endif

#include <boost/filesystem.hpp>
#include <iostream>

namespace illumestimators {

Metadata::Metadata()
{

}

Metadata::Metadata(const std::string& filename) :
	m_filename(filename)
{

}

Metadata::~Metadata()
{

}

cv::Mat_<cv::Vec3d> Metadata::illuminantMap() const
{
	cv::Mat_<cv::Vec3d> illuminantMap = cv::imread(m_illuminantMapFilename);
	return illuminantMap;
}

cv::Mat_<unsigned char> Metadata::mask() const
{
	cv::Mat_<unsigned char> mask = cv::imread(m_maskFilename, 0);
	return mask;
}

cv::Mat_<unsigned char> Metadata::target() const
{
	cv::Mat_<unsigned char> target = cv::imread(m_targetFilename, 0);
	return target;
}

void Metadata::print() const
{
	std::cout << "Metadata:" << std::endl;
	std::cout << "filename           : " << m_filename << std::endl;
	std::cout << "database           : " << m_database << std::endl;
	std::cout << "scene              : " << m_scene << std::endl;
	std::cout << "colorspace         : " << m_colorspace << std::endl;
	std::cout << "light sources      : " << m_lightSources << std::endl;
		std::cout << "dominant illuminant: " << "r = " << m_dominantIlluminant.r << ", g = " << m_dominantIlluminant.g << ", b = " << m_dominantIlluminant.b << ", "<< std::endl;
	std::cout << "illuminant map     : " << m_illuminantMapFilename << std::endl;
	std::cout << "mask               : " << m_maskFilename << std::endl;
	std::cout << "target             : " << m_targetFilename << std::endl;
	std::cout << std::endl;
}

bool Metadata::parse(std::vector<std::string> labels, std::vector<std::string> values, const std::string& parentPath)
{
	if (labels.size() != values.size()) {
		return false;
	}

	for (unsigned int i = 0; i < labels.size(); i++) {
		std::string label = labels[i];
		std::string value = values[i];

		if (label.compare("filename") == 0) {
			m_filename = fixPath(value, parentPath);
		} else if (label.compare("database") == 0) {
			m_database = value;
		} else if (label.compare("scene") == 0) {
			m_scene = value;
		} else if (label.compare("colorspace") == 0) {
			m_colorspace = value;
		} else if (label.compare("lightsources") == 0) {
			m_lightSources = value;
		} else if (label.compare("r") == 0) {
			std::istringstream iss(value);
			iss >> m_dominantIlluminant.r;
		} else if (label.compare("g") == 0) {
			std::istringstream iss(value);
			iss >> m_dominantIlluminant.g;
		} else if (label.compare("b") == 0) {
			std::istringstream iss(value);
			iss >> m_dominantIlluminant.b;
		} else if (label.compare("illuminantmap") == 0) {
			m_illuminantMapFilename = fixPath(value, parentPath);
		} else if (label.compare("mask") == 0) {
			m_maskFilename = fixPath(value, parentPath);
		} else if (label.compare("target") == 0) {
			m_targetFilename = fixPath(value, parentPath);
		}
	}

	if (m_filename.size() == 0) {
		return false;
	}

	return true;
}

std::string Metadata::fixPath(const std::string& path, const std::string& parentPath) const
{
	std::string fixedPath;

	if (boost::filesystem::path(path).is_absolute()) {
		fixedPath = path;
	} else {
		fixedPath = parentPath + "/" + path;
	}

	/*char* actualpath = realpath(fixedPath.c_str(), NULL);
	if (!actualpath) {
		std::cerr << "The path \"" << path << "\" cannot be resolved!" << std::endl;
		return std::string();
	}
	fixedPath = actualpath;
	free(actualpath);*/

	return fixedPath;
}

} // namespace illumestimators
