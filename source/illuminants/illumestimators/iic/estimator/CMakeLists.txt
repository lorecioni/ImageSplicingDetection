vole_module_name("iic")
vole_module_description("Inverse Intensity-Chromaticity-based illuminant estimators ")
vole_module_variable("Vole_IIC")

vole_add_required_dependencies("OPENCV")
vole_add_required_modules("illumestimators" "superpixels")

vole_compile_library(
	"iic.h"            "iic.cpp"
	"tan.h"            "tan.cpp"
	"iebv.h"           "iebv.cpp" 
	"iebv_estimator.h" "iebv_estimator.cpp"
)


vole_add_module()
