#include "command.h"

/* Caching Super Class */
#include "command_cache_adm.h"
/* Base Methods for reflectance analysis, e.g. helper methods for reading images */
#include "commands/command_srgb2rgb.h"
/* Algorithms for Superpixel Segmentation */
#include "commands/commandfelzenszwalb.h"
/* Algorithms for Superpixel Segmentation */
#include "commands/commandveksler.h"
/* Illuminant Estimation Algorithms */
#include "commands/grayworldcommand.h"
/* Illuminant Estimation Algorithms */
#include "commands/bayesiancommand.h"
/* Command Interface for IIC-based illuminant estimators */
#include "command_iebv2.h"
/* Command Interface for IIC-based illuminant estimators */
#include "command_test_iebv.h"
/* Command Interface for IIC-based illuminant estimators */
#include "command_test_cache.h"
/* Evaluation helper commands for IIC-based methods */
#include "command_iic_funt_eval.h"
/* Local Illuminant Estimation */
#include "commands/command_color_illum_field.h"
/* Local Illuminant Estimation */
#include "commands/command_liebv.h"
/* Local Illuminant Estimation */
#include "commands/command_ltan.h"
/* Local Illuminant Estimation */
#include "commands/command_tan_orig.h"
/* Local Illuminant Estimation */
#include "commands/command_gw_orig.h"
/* Local Illuminant Estimation */
#include "commands/command_lgrayworld.h"
/* Local Illuminant Estimation */
#include "commands/command_lbayesian.h"

#include <string>
#include <map>

namespace vole {

class Modules : public std::map<const std::string, Command *> {

public:
	typedef std::map<const std::string, Command *>::iterator iterator;
	typedef std::map<const std::string, Command *>::const_iterator const_iterator;

	Modules(); // see generated_commands_template.txt for implementation
	~Modules() {
		for (const_iterator it = begin(); it != end(); ++it)
			delete it->second;
	}
};

}

