# ---------------------------------
#	[in] 	MIMALLOC_PATH						- Root directory mimalloc
#	[out] 	MIMALLOC_INCLUDE					- Directory with includes
#	[out]	MIMALLOC_LIBS						- Libs mimalloc to link
# 	[out] 	MIMALLOC_INSTALL_LICENSES			- Mimalloc licenses to install
# 	[out] 	MIMALLOC_INSTALL_DLLS_DEBUG 		- Mimalloc files to install (Debug)
# 	[out] 	MIMALLOC_INSTALL_DLLS_RELEASE 		- Mimalloc files to install (Release)
#	[out]	MIMALLOC_FOUND						- Is found mimalloc
# ---------------------------------

set( MIMALLOC_FOUND 			OFF )
set( MIMALLOC_SEARCH_PATHS 		${MIMALLOC_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( MIMALLOC_LIB_PATH_SUFFIXES		"lib/win64" )
		set( MIMALLOC_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( MIMALLOC_LIB_PATH_SUFFIXES		"lib/win32" )
		set( MIMALLOC_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Mimalloc :: Unknown bit platform" )
		return()
	endif()

	set( MIMALLOC_LIB_NAMES 		"mimalloc" )
	set( MIMALLOC_DLL_NAMES 		"mimalloc.dll" )

# Unknown platform
else()
	message( SEND_ERROR "Mimalloc :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		MIMALLOC_INCLUDE
				NO_DEFAULT_PATH
				NAMES "mimalloc.h"
				PATH_SUFFIXES include
				PATHS ${MIMALLOC_SEARCH_PATHS} )

# Libs
find_library( 	MIMALLOC_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${MIMALLOC_LIB_NAMES}
				PATH_SUFFIXES ${MIMALLOC_LIB_PATH_SUFFIXES}/debug
				PATHS ${MIMALLOC_SEARCH_PATHS} )
find_library( 	MIMALLOC_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${MIMALLOC_LIB_NAMES}
				PATH_SUFFIXES ${MIMALLOC_LIB_PATH_SUFFIXES}/release
				PATHS ${MIMALLOC_SEARCH_PATHS} )

# DLLs
find_file( 		MIMALLOC_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${MIMALLOC_DLL_NAMES}
				PATH_SUFFIXES ${MIMALLOC_BIN_PATH_SUFFIXES}/debug
                PATHS ${MIMALLOC_SEARCH_PATHS} )
find_file( 		MIMALLOC_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${MIMALLOC_DLL_NAMES}
				PATH_SUFFIXES ${MIMALLOC_BIN_PATH_SUFFIXES}/release
                PATHS ${MIMALLOC_SEARCH_PATHS} )

# Licenses
find_file( 		MIMALLOC_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${MIMALLOC_SEARCH_PATHS} )


if ( NOT MIMALLOC_INCLUDE OR NOT MIMALLOC_LIB_DEBUG OR NOT MIMALLOC_LIB_RELEASE OR NOT MIMALLOC_DEBUG_DLL OR NOT MIMALLOC_RELEASE_DLL )
	message( SEND_ERROR "Mimalloc :: Not found" )
	return()
else()
	set( MIMALLOC_FOUND 				ON )
	set( MIMALLOC_LIBS					$<$<CONFIG:Debug>:${MIMALLOC_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${MIMALLOC_LIB_RELEASE}> )
	set( MIMALLOC_INSTALL_LICENSES		${MIMALLOC_LICENSE} )
	set( MIMALLOC_INSTALL_DLLS_DEBUG 	${MIMALLOC_DEBUG_DLL} )
	set( MIMALLOC_INSTALL_DLLS_RELEASE 	${MIMALLOC_RELEASE_DLL} )
endif()
