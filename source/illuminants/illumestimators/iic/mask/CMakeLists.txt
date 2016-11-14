vole_module_name("iic_mask")
vole_module_description("Masking Interface for IIC-based illuminant estimators ")
vole_module_variable("Vole_IICmask")

vole_add_required_dependencies("OPENCV")
vole_add_required_modules()

vole_compile_library(
	"mask.cpp" "mask.h"
	"lehmann_palm.cpp" "lehmann_palm.h"
)


vole_add_module()
