vole_module_name("iic_misc")
vole_module_description("Helper classes, not intended for end user build")
vole_module_variable("Vole_IIC_misc")

vole_add_required_dependencies("OPENCV" "BOOST" "BOOST_FILESYSTEM") # "BOOST_PROPERTY_TREE" "BOOST_CIRCULAR_BUFFER")
vole_add_required_modules()

vole_compile_library(
	"get_file.h"              "get_file.cpp"
)


vole_add_module()
