vole_module_name("iic_commands")
vole_module_description("Command Interface for IIC-based illuminant estimators")
vole_module_variable("Vole_IICcommands")

vole_add_required_dependencies("OPENCV")
vole_add_required_modules("superpixels" "iic" "iic_misc" "iic_eval")

vole_compile_library(
	"command_iebv.cpp"       "command_iebv2.h"
	"command_test_iebv.cpp"  "command_test_iebv.h"
	"config_iebv.cpp"        "config_iebv.h"
	"command_test_cache.cpp" "command_test_cache.h" 
)

vole_add_command("iebv2"              "command_iebv2.h"              "iic::CommandIebv")
vole_add_command("iebvTest"           "command_test_iebv.h"          "iic::CommandTestIebv")
vole_add_command("test_cache"         "command_test_cache.h"         "iic::CommandTestCache")


vole_add_module()
