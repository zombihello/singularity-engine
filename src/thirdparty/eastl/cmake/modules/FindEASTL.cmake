# ---------------------------------
#	[in] 	EASTL_PATH						- Root directory EASTL
#	[out] 	EASTL_INCLUDE					- Directory with includes
#	[out]	EASTL_LIBS						- Libs EASTL to link
#	[out]	EASTL_VISUALIZERS				- EASTL visualizers for debugger
# 	[out] 	EASTL_INSTALL_LICENSES			- EASTL licenses to install
#	[out]	EASTL_FOUND						- Is found EASTL
# ---------------------------------

set( EASTL_FOUND 			OFF )
set( EASTL_SEARCH_PATHS 	${EASTL_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( EASTL_LIB_PATH_SUFFIXES		"lib/win64" )
	elseif( PLATFORM_32BIT )
		set( EASTL_LIB_PATH_SUFFIXES		"lib/win32" )
	else()
		message( SEND_ERROR "EASTL :: Unknown bit platform" )
		return()
	endif()

	set( EASTL_LIB_NAMES 		"EASTL" )

# Unknown platform
else()
	message( SEND_ERROR "EASTL :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		EASTL_INCLUDE
				NO_DEFAULT_PATH
				NAMES "EASTL/version.h"
				PATH_SUFFIXES include
				PATHS ${EASTL_SEARCH_PATHS} )

# Libs
find_library( 	EASTL_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${EASTL_LIB_NAMES}
				PATH_SUFFIXES ${EASTL_LIB_PATH_SUFFIXES}/debug
                PATHS ${EASTL_SEARCH_PATHS} )
find_library( 	EASTL_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${EASTL_LIB_NAMES}
				PATH_SUFFIXES ${EASTL_LIB_PATH_SUFFIXES}/release
                PATHS ${EASTL_SEARCH_PATHS} )

# Visualizers
find_file( 		EASTL_VISUALIZER
				NO_DEFAULT_PATH
				NAMES "EASTL.natvis"
				PATH_SUFFIXES doc
				PATHS ${EASTL_SEARCH_PATHS} )

# Licenses
find_file( 		EASTL_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${EASTL_SEARCH_PATHS} )


if ( NOT EASTL_INCLUDE OR NOT EASTL_LIB_DEBUG OR NOT EASTL_LIB_RELEASE OR NOT EASTL_VISUALIZER )
	message( SEND_ERROR "EASTL :: Not found" )
	return()
else()
	set( EASTL_FOUND 				ON )
	set( EASTL_LIBS 				$<$<CONFIG:Debug>:${EASTL_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${EASTL_LIB_RELEASE}> )
	set( EASTL_VISUALIZERS			${EASTL_VISUALIZER} )
	set( EASTL_INSTALL_LICENSES		${EASTL_LICENSE} )
endif()