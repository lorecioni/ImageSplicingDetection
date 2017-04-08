cmake_minimum_required(VERSION 2.8)
project(Vole)

find_package( OpenCV REQUIRED )
find_package( Boost 1.43.0 COMPONENTS program_options )

include_directories(${Boost_INCLUDE_DIRS})
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_definitions(-DWITH_BOOST -DWITH_BOOST_PROGRAM_OPTIONS)

include_directories(${OpenCV_DIR}/../opencv-Build/install/include)
include_directories(
	shell
	rbase
	superpixels
	superpixels/felzenszwalb/implementation/
	illumestimators
	lille
)

set(PROJECT_FILES
	shell/command.h
	shell/modules.h
	shell/modules.cpp
	shell/config.h
	shell/config.cpp
	illumestimators/common/statistics.h
	illumestimators/common/mask.h
	illumestimators/common/derivative.cpp
	illumestimators/common/mask.cpp
	illumestimators/common/derivative.h
	illumestimators/common/statistics.cpp
	illumestimators/common/color.cpp
	illumestimators/common/color.h
	illumestimators/illuminantestimator.cpp
	illumestimators/illuminantestimator.h
	illumestimators/fusion/fusion.h
	illumestimators/fusion/histogramVotingFusion.cpp
	illumestimators/fusion/config_histogram_voting_fusion.h
	illumestimators/fusion/config_histogram_voting_fusion.cpp
	illumestimators/fusion/histogramVotingFusion.h
	illumestimators/fusion/fusion.cpp
	illumestimators/iic/config_iebv.cpp
	illumestimators/iic/config_iebv.h
	illumestimators/iic/iic.h
	illumestimators/iic/iic.cpp
	illumestimators/iic/iebv.cpp
	illumestimators/iic/iebv_estimator.h
	illumestimators/iic/iebv_estimator.cpp
	illumestimators/iic/iebv.h
	illumestimators/grayworld/grayworldestimator.h
	illumestimators/grayworld/grayworldestimator.cpp
	lille/commands/command_lgrayworld.cpp
	lille/commands/command_liebv.h
	lille/commands/command_liebv.cpp
	lille/commands/command_lgrayworld.h
	lille/paint_superpixel.h
	lille/liebv_config.cpp
	lille/lgrayworld_config.h
	lille/liebv_config.h
	lille/lgrayworld_config.cpp
	lille/paint_superpixel.cpp
	superpixels/felzenszwalb/implementation/image.h
	superpixels/felzenszwalb/implementation/misc.h
	superpixels/felzenszwalb/implementation/segment-image.h
	superpixels/felzenszwalb/implementation/filter.h
	superpixels/felzenszwalb/implementation/segment-graph.h
	superpixels/felzenszwalb/implementation/disjoint-set.h
	superpixels/felzenszwalb/implementation/convolve.h
	superpixels/felzenszwalb/implementation/imconv.h
	superpixels/felzenszwalb/felzenszwalbsegmentation.h
	superpixels/felzenszwalb/felzenszwalbsegmentation.cpp
	superpixels/normal_image.h
	superpixels/commands/commandfelzenszwalb.cpp
	superpixels/commands/patchesconfig.cpp
	superpixels/commands/patchesconfig.h
	superpixels/commands/commandfelzenszwalb.h
	superpixels/commands/felzenszwalbconfig.cpp
	superpixels/commands/felzenszwalbconfig.h
	superpixels/superpixel.h
	superpixels/superpixelsegmentation.h
	superpixels/patches/patchessegmentation.h
	superpixels/patches/patchessegmentation.cpp
	superpixels/normal_image.cpp
	superpixels/superpixelsegmentation.cpp
	rbase/illum.h
	rbase/illum.cpp
	rbase/color_space.h
	rbase/color_space.cpp
	rbase/img_read_config.h
	rbase/img_read.h
	rbase/img_read.cpp
	rbase/img_read_config.cpp
	rbase/color.cpp
	rbase/color.h
)

add_executable( 
	vole shell/main.cxx ${PROJECT_FILES}
)
target_link_libraries(vole ${OpenCV_LIBS} ${Boost_LIBRARIES})

