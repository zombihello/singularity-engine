# ---------------------------------
#	[in] 	FLECS_PATH						- Root directory Flecs
#	[out] 	FLECS_INCLUDE					- Directory with includes
#	[out]	FLECS_LIBS						- Libs Flecs to link
#	[out]	FLECS_DEFINES					- Flecs defines to set
# 	[out] 	FLECS_INSTALL_LICENSES			- Flecs licenses to install
# 	[out] 	FLECS_INSTALL_DLLS_DEBUG 		- Flecs DLLs to install (Debug)
# 	[out] 	FLECS_INSTALL_DLLS_RELEASE 		- Flecs DLLs to install (Release)
#	[out]	FLECS_FOUND						- Is found Flecs
# ---------------------------------

set( FLECS_FOUND 			OFF )
set( FLECS_SEARCH_PATHS 	${FLECS_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( FLECS_LIB_PATH_SUFFIXES		"lib/win64" )
		set( FLECS_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( FLECS_LIB_PATH_SUFFIXES		"lib/win32" )
		set( FLECS_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Flecs :: Unknown bit platform" )
		return()
	endif()

	set( FLECS_LIB_NAMES 		"flecs" )
	set( FLECS_DLL_NAMES 		"flecs.dll" )

# Unknown platform
else()
	message( SEND_ERROR "Flecs :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		FLECS_INCLUDE
				NO_DEFAULT_PATH
				NAMES "flecs.h"
				PATH_SUFFIXES include
				PATHS ${FLECS_SEARCH_PATHS} )

# Libs
find_library( 	FLECS_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${FLECS_LIB_NAMES}
				PATH_SUFFIXES ${FLECS_LIB_PATH_SUFFIXES}/debug
                PATHS ${FLECS_SEARCH_PATHS} )
find_library( 	FLECS_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${FLECS_LIB_NAMES}
				PATH_SUFFIXES ${FLECS_LIB_PATH_SUFFIXES}/release
                PATHS ${FLECS_SEARCH_PATHS} )
# DLLs
find_file( 		FLECS_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${FLECS_DLL_NAMES}
				PATH_SUFFIXES ${FLECS_BIN_PATH_SUFFIXES}/debug
                PATHS ${FLECS_SEARCH_PATHS} )
find_file( 		FLECS_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${FLECS_DLL_NAMES}
				PATH_SUFFIXES ${FLECS_BIN_PATH_SUFFIXES}/release
                PATHS ${FLECS_SEARCH_PATHS} )

# Licenses
find_file( 		FLECS_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${FLECS_SEARCH_PATHS} )


if ( NOT FLECS_INCLUDE OR NOT FLECS_LIB_DEBUG OR NOT FLECS_LIB_RELEASE OR NOT FLECS_DEBUG_DLL OR NOT FLECS_RELEASE_DLL )
	message( SEND_ERROR "Flecs :: Not found" )
	return()
else()
	set( FLECS_FOUND 					ON )
	set( FLECS_DEFINES					FLECS_CPP_NO_AUTO_REGISTRATION )
	set( FLECS_LIBS 					$<$<CONFIG:Debug>:${FLECS_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${FLECS_LIB_RELEASE}> )
	set( FLECS_INSTALL_LICENSES			${FLECS_LICENSE} )
	set( FLECS_INSTALL_DLLS_DEBUG 		${FLECS_DEBUG_DLL} )
	set( FLECS_INSTALL_DLLS_RELEASE 	${FLECS_RELEASE_DLL} )
endif()