# ---------------------------------
#	[in] 	SDL2_PATH						- Root directory SDL2
#	[out] 	SDL2_INCLUDE					- Directory with includes
#	[out]	SDL2_LIBS						- Libs SDL2 to link
# 	[out] 	SDL2_INSTALL_LICENSES			- SDL2 licenses to install
# 	[out] 	SDL2_INSTALL_DLLS_DEBUG 		- SDL2 DLLs to install (Debug)
# 	[out] 	SDL2_INSTALL_DLLS_RELEASE 		- SDL2 DLLs to install (Release)
#	[out]	SDL2_FOUND						- Is found SDL2
# ---------------------------------

set( SDL2_FOUND 			OFF )
set( SDL2_SEARCH_PATHS 		${SDL2_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( SDL2_LIB_PATH_SUFFIXES		"lib/win64" )
		set( SDL2_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( SDL2_LIB_PATH_SUFFIXES		"lib/win32" )
		set( SDL2_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "SDL2 :: Unknown bit platform" )
		return()
	endif()

	set( SDL2_DEBUG_LIB_NAMES 		"SDL2d" )
	set( SDL2_DEBUG_DLL_NAMES 		"SDL2d.dll" )
	set( SDL2_RELEASE_LIB_NAMES 	"SDL2" )
	set( SDL2_RELEASE_DLL_NAMES 	"SDL2.dll" )

# Unknown platform
else()
	message( SEND_ERROR "SDL2 :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		SDL2_INCLUDE
				NO_DEFAULT_PATH
				NAMES "SDL2/SDL.h"
				PATH_SUFFIXES include
				PATHS ${SDL2_SEARCH_PATHS} )

# Libs
find_library( 	SDL2_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${SDL2_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${SDL2_LIB_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )
find_library( 	SDL2_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${SDL2_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${SDL2_LIB_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )

# DLLs
find_file( 		SDL2_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${SDL2_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${SDL2_BIN_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )
find_file( 		SDL2_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${SDL2_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${SDL2_BIN_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )

# Licenses
find_file( 		SDL2_LICENSE_TXT
				NO_DEFAULT_PATH
				NAMES "LICENSE.txt"
				PATHS ${SDL2_SEARCH_PATHS} )


if ( NOT SDL2_INCLUDE OR NOT SDL2_LIB_DEBUG OR NOT SDL2_LIB_RELEASE OR NOT SDL2_DEBUG_DLL OR NOT SDL2_RELEASE_DLL )
	message( SEND_ERROR "SDL2 :: Not found" )
	return()
else()
	set( SDL2_FOUND 				ON )
	set( SDL2_LIBS					$<$<CONFIG:Debug>:${SDL2_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${SDL2_LIB_RELEASE}> )
	set( SDL2_INSTALL_LICENSES		${SDL2_LICENSE_TXT} )
	set( SDL2_INSTALL_DLLS_DEBUG 	${SDL2_DEBUG_DLL} )
	set( SDL2_INSTALL_DLLS_RELEASE 	${SDL2_RELEASE_DLL} )
endif()