# ---------------------------------
#	[in] 	GLM_PATH					- Root directory GLM
#	[out] 	GLM_INCLUDE					- Directory with includes
# 	[out] 	GLM_INSTALL_LICENSES		- GLM licenses to install
#	[out]	GLM_FOUND					- Is found GLM
# ---------------------------------

set( GLM_FOUND 			OFF )
set( GLM_SEARCH_PATHS 	${GLM_PATH} )


#
# Find pathes
#
# Includes
find_path( 		GLM_INCLUDE
				NO_DEFAULT_PATH
				NAMES "glm.hpp"
				PATH_SUFFIXES src/glm
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
	set( GLM_INSTALL_LICENSES 	${GLM_COPYING_TXT} )
endif()
