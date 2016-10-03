/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef SUPERPIXELS_COMMANDS_FELZENSZWALBCONFIG
#define SUPERPIXELS_COMMANDS_FELZENSZWALBCONFIG

#include "config.h"

namespace vole {

class FelzenszwalbConfig : public Config {
public:
	FelzenszwalbConfig(const std::string& prefix = std::string());

public:
	virtual std::string getString() const;

protected:
	#ifdef WITH_BOOST
		virtual void initBoostOptions();
	#endif // WITH_BOOST

public:
	std::string input_file;
	std::string input_file_16_bit;
	double max_intensity; // maximum intensity for a pixel
	double min_intensity; // minimum intensity for a pixel

	std::string output_file;

	bool deterministic_coloring;
	bool isGraphical;
	int kThreshold;
	double sigma;
	int min_size;
};

} // vole

#endif // SUPERPIXELS_COMMANDS_FELZENSZWALBCONFIG
