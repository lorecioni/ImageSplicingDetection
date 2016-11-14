#ifndef LILLE_LIEBV_CONFIG_H
#define LILLE_LIEBV_CONFIG_H

#include "config.h"
#include "img_read_config.h"

namespace lille {

	class LIebvConfig : public vole::Config {
		public:
			LIebvConfig(const std::string& prefix = std::string());
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

			std::string iebvConfigFile;
	};
}

#endif //  LILLE_LIEBV_CONFIG_H
