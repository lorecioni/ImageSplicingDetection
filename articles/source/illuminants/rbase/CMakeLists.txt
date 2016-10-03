vole_module_name("rbase")
vole_module_description("Base Methods for reflectance analysis, e.g. helper methods for reading images")
vole_module_variable("Vole_RBase")

vole_add_required_dependencies("OPENCV")
vole_add_required_modules()

vole_compile_library(
	"shi_db_entry.h"              "shi_db_entry.cpp"
	"db_index_shi.h"              "db_index_shi.cpp"
	"db_index_shida.h"            "db_index_shida.cpp"
	"db_descriptor.h"             "db_descriptor.cpp"
	"img_read.h"                  "img_read.cpp"
	"img_read_config.h"           "img_read_config.cpp"
	"color_space.h"               "color_space.cpp"
	"mask.h"                      "mask.cpp"
	"illum.h"                     "illum.cpp"
	"color.h"                     "color.cpp"
	"commands/command_srgb2rgb.h" "commands/command_srgb2rgb.cpp"
	"srgb2rgb_config.h"           "srgb2rgb_config.cpp"
)

vole_add_command("srgb2rgb" "commands/command_srgb2rgb.h" "rbase::CommandSrgb2Rgb")

vole_add_module()
