# ---------------------------------
#	[in] 	WILDMATCH_PATH						- Root directory wildmatch
#	[out] 	WILDMATCH_INCLUDE					- Directory with includes
#	[out]	WILDMATCH_LIBS						- Libs wildmatch to link
# 	[out] 	WILDMATCH_INSTALL_LICENSES			- Wildmatch licenses to install
#	[out]	WILDMATCH_FOUND						- Is found WILDMATCH
# ---------------------------------

set( WILDMATCH_FOUND 			OFF )
set( WILDMATCH_SEARCH_PATHS 	${WILDMATCH_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( WILDMATCH_LIB_PATH_SUFFIXES		"lib/win64" )
	elseif( PLATFORM_32BIT )
		set( WILDMATCH_LIB_PATH_SUFFIXES		"lib/win32" )
	else()
		message( SEND_ERROR "Wildmatch :: Unknown bit platform" )
		return()
	endif()

	set( WILDMATCH_LIB_NAMES 		"wildmatch" )

# Unknown platform
else()
	message( SEND_ERROR "Wildmatch :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		WILDMATCH_INCLUDE
				NO_DEFAULT_PATH
				NAMES "wildmatch/wildmatch.h"
				PATH_SUFFIXES include
				PATHS ${WILDMATCH_SEARCH_PATHS} )

# Libs
find_library( 	WILDMATCH_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${WILDMATCH_LIB_NAMES}
				PATH_SUFFIXES ${WILDMATCH_LIB_PATH_SUFFIXES}/debug
                PATHS ${WILDMATCH_SEARCH_PATHS} )
find_library( 	WILDMATCH_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${WILDMATCH_LIB_NAMES}
				PATH_SUFFIXES ${WILDMATCH_LIB_PATH_SUFFIXES}/release
                PATHS ${WILDMATCH_SEARCH_PATHS} )

# Licenses
find_file( 		WILDMATCH_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${WILDMATCH_SEARCH_PATHS} )


if ( NOT WILDMATCH_INCLUDE OR NOT WILDMATCH_LIB_DEBUG OR NOT WILDMATCH_LIB_RELEASE )
	message( SEND_ERROR "Wildmatch :: Not found" )
	return()
else()
	set( WILDMATCH_FOUND 				ON )
	set( WILDMATCH_LIBS 				$<$<CONFIG:Debug>:${WILDMATCH_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${WILDMATCH_LIB_RELEASE}> )
	set( WILDMATCH_INSTALL_LICENSES		${WILDMATCH_LICENSE} )
endif()