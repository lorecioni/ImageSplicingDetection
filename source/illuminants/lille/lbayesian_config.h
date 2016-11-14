#ifndef LILLE_LBAYESIAN_CONFIG_H
#define LILLE_LBAYESIAN_CONFIG_H

#include "config.h"
#include "img_read_config.h"

namespace lille {

	class LBayesianConfig : public vole::Config {
		public:
			LBayesianConfig(const std::string& prefix = std::string());
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
