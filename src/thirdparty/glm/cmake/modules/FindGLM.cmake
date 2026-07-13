# ---------------------------------
#	[in] 	GLM_PATH					- Root directory GLM
#	[out] 	GLM_INCLUDE					- Directory with includes
#	[out]	GLM_DEFINES					- GLM defines to set
#	[out]	GLM_VISUALIZERS				- GLM visualizers for debugger
# 	[out] 	GLM_INSTALL_LICENSES		- GLM licenses to install
#	[out]	GLM_FOUND					- Is found GLM
# ---------------------------------

set( GLM_FOUND 			OFF )
set( GLM_SEARCH_PATHS 	${GLM_PATH} )
set( GLM_DEFINES 		GLM_ENABLE_EXPERIMENTAL GLM_FORCE_INTRINSICS GLM_FORCE_DEFAULT_ALIGNED_GENTYPES GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE )


#
# Find pathes
#
# Includes
find_path( 		GLM_INCLUDE
				NO_DEFAULT_PATH
				NAMES "glm.hpp"
				PATH_SUFFIXES src/glm
				PATHS ${GLM_SEARCH_PATHS} )

# Visualizers
find_file( 		GLM_VISUALIZER
				NO_DEFAULT_PATH
				NAMES "glm.natvis"
				PATH_SUFFIXES src/util
				PATHS ${GLM_SEARCH_PATHS} )

# Licenses
find_file( 		GLM_COPYING_TXT
				NO_DEFAULT_PATH
				NAMES "copying.txt"
				PATHS ${GLM_SEARCH_PATHS} )


if ( NOT GLM_INCLUDE )
	message( SEND_ERROR "GLM :: Not found" )
	return()
else()
	set( GLM_FOUND 				ON )
	set( GLM_VISUALIZERS		${GLM_VISUALIZER} )
	set( GLM_INSTALL_LICENSES 	${GLM_COPYING_TXT} )
endif()
