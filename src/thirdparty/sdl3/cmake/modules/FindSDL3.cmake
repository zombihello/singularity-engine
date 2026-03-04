# ---------------------------------
#	[in] 	SDL3_PATH						- Root directory SDL3
#	[out] 	SDL3_INCLUDE					- Directory with includes
#	[out]	SDL3_LIBS						- Libs SDL3 to link
# 	[out] 	SDL3_INSTALL_LICENSES			- SDL3 licenses to install
# 	[out] 	SDL3_INSTALL_DLLS_DEBUG 		- SDL3 DLLs to install (Debug)
# 	[out] 	SDL3_INSTALL_DLLS_RELEASE 		- SDL3 DLLs to install (Release)
#	[out]	SDL3_FOUND						- Is found SDL3
# ---------------------------------

set( SDL3_FOUND 			OFF )
set( SDL3_SEARCH_PATHS 		${SDL3_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( SDL3_LIB_PATH_SUFFIXES		"lib/win64" )
		set( SDL3_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( SDL3_LIB_PATH_SUFFIXES		"lib/win32" )
		set( SDL3_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "SDL3 :: Unknown bit platform" )
		return()
	endif()

	set( SDL3_DEBUG_LIB_NAMES 		"SDL3d" )
	set( SDL3_DEBUG_DLL_NAMES 		"SDL3d.dll" )
	set( SDL3_RELEASE_LIB_NAMES 	"SDL3" )
	set( SDL3_RELEASE_DLL_NAMES 	"SDL3.dll" )

# Unknown platform
else()
	message( SEND_ERROR "SDL3 :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		SDL3_INCLUDE
				NO_DEFAULT_PATH
				NAMES "SDL3/SDL.h"
				PATH_SUFFIXES include
				PATHS ${SDL3_SEARCH_PATHS} )

# Libs
find_library( 	SDL3_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${SDL3_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${SDL3_LIB_PATH_SUFFIXES}/debug
                PATHS ${SDL3_SEARCH_PATHS} )
find_library( 	SDL3_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${SDL3_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${SDL3_LIB_PATH_SUFFIXES}/release
                PATHS ${SDL3_SEARCH_PATHS} )

# DLLs
find_file( 		SDL3_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${SDL3_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${SDL3_BIN_PATH_SUFFIXES}/debug
                PATHS ${SDL3_SEARCH_PATHS} )
find_file( 		SDL3_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${SDL3_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${SDL3_BIN_PATH_SUFFIXES}/release
                PATHS ${SDL3_SEARCH_PATHS} )

# Licenses
find_file( 		SDL3_LICENSE_TXT
				NO_DEFAULT_PATH
				NAMES "LICENSE.txt"
				PATHS ${SDL3_SEARCH_PATHS} )


if ( NOT SDL3_INCLUDE OR NOT SDL3_LIB_DEBUG OR NOT SDL3_LIB_RELEASE OR NOT SDL3_DEBUG_DLL OR NOT SDL3_RELEASE_DLL )
	message( SEND_ERROR "SDL3 :: Not found" )
	return()
else()
	set( SDL3_FOUND 				ON )
	set( SDL3_LIBS					$<$<CONFIG:Debug>:${SDL3_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${SDL3_LIB_RELEASE}> )
	set( SDL3_INSTALL_LICENSES		${SDL3_LICENSE_TXT} )
	set( SDL3_INSTALL_DLLS_DEBUG 	${SDL3_DEBUG_DLL} )
	set( SDL3_INSTALL_DLLS_RELEASE 	${SDL3_RELEASE_DLL} )
endif()