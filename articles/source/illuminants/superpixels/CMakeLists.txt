vole_module_name("superpixels")
vole_module_description("Algorithms for Superpixel Segmentation")
vole_module_variable("Vole_Superpixels")

vole_add_required_dependencies("OPENCV" "BOOST" "BOOST_PROGRAM_OPTIONS" "BOOST_SERIALIZATION")
vole_add_required_modules("cache" "rbase")

vole_compile_library(
	commands/commandfelzenszwalb.cpp
	commands/commandfelzenszwalb.h
	commands/felzenszwalbconfig.cpp
	commands/felzenszwalbconfig.h

	commands/commandpatches.cpp
	commands/commandpatches.h
	commands/patchesconfig.cpp
	commands/patchesconfig.h

	commands/commandveksler.cpp
	commands/commandveksler.h
	commands/vekslerconfig.cpp
	commands/vekslerconfig.h
)

vole_add_command("felzenszwalb" "commands/commandfelzenszwalb.h" "vole::CommandFelzenszwalb")
vole_add_command("veksler" "commands/commandveksler.h" "vole::CommandVeksler")

vole_compile_library(
	superpixel.h
	superpixelsegmentation.cpp
	superpixelsegmentation.h

	normal_image.cpp
	normal_image.h

	felzenszwalb/felzenszwalbsegmentation.cpp
	felzenszwalb/felzenszwalbsegmentation.h

	felzenszwalb/implementation/image.h
	felzenszwalb/implementation/misc.h
	felzenszwalb/implementation/segment-image.h
	felzenszwalb/implementation/filter.h
	felzenszwalb/implementation/segment-graph.h
	felzenszwalb/implementation/disjoint-set.h
	felzenszwalb/implementation/convolve.h
	felzenszwalb/implementation/imconv.h

	patches/patchessegmentation.cpp
	patches/patchessegmentation.h

	veksler/vekslersegmentation.cpp
	veksler/vekslersegmentation.h

	veksler/implementation/maxflow.cpp
	veksler/implementation/graph.cpp
	veksler/implementation/graph.h
	veksler/implementation/superpixels.cpp
	veksler/implementation/superpixels.h
	veksler/implementation/block.h
	veksler/implementation/energy.h

	veksler/implementation/vlib/include/canny.h
	veksler/implementation/vlib/include/draw.h
	veksler/implementation/vlib/include/filter.h
	veksler/implementation/vlib/include/imconv.h
	veksler/implementation/vlib/include/misc.h
	veksler/implementation/vlib/include/point.h
	veksler/implementation/vlib/include/steer.h
	veksler/implementation/vlib/include/chamfer.h
	veksler/implementation/vlib/include/dt.h
	veksler/implementation/vlib/include/grad.h
	veksler/implementation/vlib/include/imutil.h
	veksler/implementation/vlib/include/morph.h
	veksler/implementation/vlib/include/pyramid.h
	veksler/implementation/vlib/include/convolve.h
	veksler/implementation/vlib/include/features.h
	veksler/implementation/vlib/include/image.h
	veksler/implementation/vlib/include/matrix.h
	veksler/implementation/vlib/include/sample.h
)

vole_add_module()
