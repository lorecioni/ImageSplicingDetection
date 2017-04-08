/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef IREAD_IMG_READ_CONFIG_H
#define IREAD_IMG_READ_CONFIG_H

#include "config.h"

namespace iread {

	class IReadConfig : public vole::Config {
		public:
			IReadConfig(const std::string& prefix = std::string());
		public:
			virtual std::string getString() const;

		protected:
			#ifdef WITH_BOOST
				virtual void initBoostOptions();
			#endif // WITH_BOOST

		public:
			std::string imageFile;

			int dark_level;
			int saturation_level;
			std::string color_space;

		public:
	};
}

#endif //  IREAD_IMG_READ_CONFIG_H
