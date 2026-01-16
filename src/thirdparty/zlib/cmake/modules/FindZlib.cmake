# ---------------------------------
#	[in] 	ZLIB_PATH						- Root directory zlib
#	[out] 	ZLIB_INCLUDE					- Directory with includes
#	[out]	ZLIB_LIBS						- Libs zlib to link
# 	[out] 	ZLIB_INSTALL_LICENSES			- Zlib licenses to install
# 	[out] 	ZLIB_INSTALL_DLLS_DEBUG 		- Zlib DLLs to install (Debug)
# 	[out] 	ZLIB_INSTALL_DLLS_RELEASE 		- Zlib DLLs to install (Release)
#	[out]	ZLIB_FOUND						- Is found zlib
# ---------------------------------

set( ZLIB_FOUND 			OFF )
set( ZLIB_SEARCH_PATHS 		${ZLIB_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( ZLIB_LIB_PATH_SUFFIXES		"lib/win64" )
		set( ZLIB_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( ZLIB_LIB_PATH_SUFFIXES		"lib/win32" )
		set( ZLIB_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Zlib :: Unknown bit platform" )
		return()
	endif()

	set( ZLIB_DEBUG_LIB_NAMES 		"zlibd" )
	set( ZLIB_DEBUG_DLL_NAMES 		"zlibd.dll" )
	set( ZLIB_RELEASE_LIB_NAMES 	"zlib" )
	set( ZLIB_RELEASE_DLL_NAMES 	"zlib.dll" )

# Unknown platform
else()
	message( SEND_ERROR "Zlib :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
# Includes
find_path( 		ZLIB_INCLUDE
				NO_DEFAULT_PATH
				NAMES "zlib.h"
				PATH_SUFFIXES include
				PATHS ${ZLIB_SEARCH_PATHS} )

# Libs
find_library( 	ZLIB_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${ZLIB_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${ZLIB_LIB_PATH_SUFFIXES}
				PATHS ${ZLIB_SEARCH_PATHS} )
find_library( 	ZLIB_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${ZLIB_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${ZLIB_LIB_PATH_SUFFIXES}
				PATHS ${ZLIB_SEARCH_PATHS} )

# DLLs
find_file( 		ZLIB_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${ZLIB_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${ZLIB_BIN_PATH_SUFFIXES}
                PATHS ${ZLIB_SEARCH_PATHS} )
find_file( 		ZLIB_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${ZLIB_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${ZLIB_BIN_PATH_SUFFIXES}
                PATHS ${ZLIB_SEARCH_PATHS} )

# Licenses
find_file( 		ZLIB_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${ZLIB_SEARCH_PATHS} )


if ( NOT ZLIB_INCLUDE OR NOT ZLIB_LIB_DEBUG OR NOT ZLIB_LIB_RELEASE OR NOT ZLIB_DEBUG_DLL OR NOT ZLIB_RELEASE_DLL )
	message( SEND_ERROR "Zlib :: Not found" )
	return()
else()
	set( ZLIB_FOUND 				ON )
	set( ZLIB_LIBS					$<$<CONFIG:Debug>:${ZLIB_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${ZLIB_LIB_RELEASE}> )
	set( ZLIB_INSTALL_LICENSES		${ZLIB_LICENSE} )
	set( ZLIB_INSTALL_DLLS_DEBUG 	${ZLIB_DEBUG_DLL} )
	set( ZLIB_INSTALL_DLLS_RELEASE 	${ZLIB_RELEASE_DLL} )
endif()
