/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef LILLE_LGRAYWORLD_CONFIG_H
#define LILLE_LGRAYWORLD_CONFIG_H

#include "config.h"
#include "img_read_config.h"

namespace lille {

	class LGrayWorldConfig : public vole::Config {
		public:
			LGrayWorldConfig(const std::string& prefix = std::string());
		public:
			virtual std::string getString() const;

		protected:
			#ifdef WITH_BOOST
				virtual void initBoostOptions();
			#endif // WITH_BOOST

		public:
			iread::IReadConfig img;
//			std::string inputFile;

			std::string outputFile;
			std::string maskFile;
			std::string segmentationFile;
//			int darklevel;

			int n;
			int p;
			double sigma;
	};
}

#endif //  LILLE_LGRAYWORLD_CONFIG_H
