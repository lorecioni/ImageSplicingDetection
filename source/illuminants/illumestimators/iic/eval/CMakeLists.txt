vole_module_name("iic_eval")
vole_module_description("Evaluation helper commands for IIC-based methods")
vole_module_variable("Vole_IICeval")

vole_add_required_dependencies("OPENCV")
vole_add_required_modules("superpixels" "iic" "iic_misc")

vole_compile_library(
	"command_iic_funt_eval.cpp"       "command_iic_funt_eval.h"
	"iic_funt_eval_config.cpp"        "iic_funt_eval_config.h"
	"plotter.cpp"                     "plotter.h"
)

vole_add_command("iic_funt"  "command_iic_funt_eval.h" "iic::FuntEval")


vole_add_module()
