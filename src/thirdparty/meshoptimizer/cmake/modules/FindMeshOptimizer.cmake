# ---------------------------------
#	[in] 	MESHOPTIMIZER_PATH						- Root directory MeshOptimizer
#	[out] 	MESHOPTIMIZER_INCLUDE					- Directory with includes
#	[out]	MESHOPTIMIZER_LIBS						- Libs MeshOptimizer to link
# 	[out] 	MESHOPTIMIZER_INSTALL_LICENSES			- MeshOptimizer licenses to install
# 	[out] 	MESHOPTIMIZER_INSTALL_DLLS_DEBUG 		- MeshOptimizer DLLs to install (Debug)
# 	[out] 	MESHOPTIMIZER_INSTALL_DLLS_RELEASE 		- MeshOptimizer DLLs to install (Release)
#	[out]	MESHOPTIMIZER_FOUND						- Is found MeshOptimizer
# ---------------------------------

set( MESHOPTIMIZER_FOUND 			OFF )
set( MESHOPTIMIZER_SEARCH_PATHS 	${MESHOPTIMIZER_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( MESHOPTIMIZER_LIB_PATH_SUFFIXES		"lib/win64" )
		set( MESHOPTIMIZER_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( MESHOPTIMIZER_LIB_PATH_SUFFIXES		"lib/win32" )
		set( MESHOPTIMIZER_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "MeshOptimizer :: Unknown bit platform" )
		return()
	endif()

	set( MESHOPTIMIZER_LIB_NAMES 		"meshoptimizer" )
	set( MESHOPTIMIZER_DLL_NAMES 		"meshoptimizer.dll" )

# Unknown platform
else()
	message( SEND_ERROR "MeshOptimizer :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		MESHOPTIMIZER_INCLUDE
				NO_DEFAULT_PATH
				NAMES "meshoptimizer.h"
				PATH_SUFFIXES include
				PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )

# Libs
find_library( 	MESHOPTIMIZER_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${MESHOPTIMIZER_LIB_NAMES}
				PATH_SUFFIXES ${MESHOPTIMIZER_LIB_PATH_SUFFIXES}/debug
                PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )
find_library( 	MESHOPTIMIZER_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${MESHOPTIMIZER_LIB_NAMES}
				PATH_SUFFIXES ${MESHOPTIMIZER_LIB_PATH_SUFFIXES}/release
                PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )
# DLLs
find_file( 		MESHOPTIMIZER_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${MESHOPTIMIZER_DLL_NAMES}
				PATH_SUFFIXES ${MESHOPTIMIZER_BIN_PATH_SUFFIXES}/debug
                PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )
find_file( 		MESHOPTIMIZER_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${MESHOPTIMIZER_DLL_NAMES}
				PATH_SUFFIXES ${MESHOPTIMIZER_BIN_PATH_SUFFIXES}/release
                PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )

# Licenses
find_file( 		MESHOPTIMIZER_LICENSE_MD
				NO_DEFAULT_PATH
				NAMES "LICENSE.md"
				PATHS ${MESHOPTIMIZER_SEARCH_PATHS} )


if ( NOT MESHOPTIMIZER_INCLUDE OR NOT MESHOPTIMIZER_LIB_DEBUG OR NOT MESHOPTIMIZER_LIB_RELEASE OR NOT MESHOPTIMIZER_DEBUG_DLL OR NOT MESHOPTIMIZER_RELEASE_DLL )
	message( SEND_ERROR "MeshOptimizer :: Not found" )
	return()
else()
	set( MESHOPTIMIZER_FOUND 					ON )
	set( MESHOPTIMIZER_LIBS 					$<$<CONFIG:Debug>:${MESHOPTIMIZER_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${MESHOPTIMIZER_LIB_RELEASE}> )
	set( MESHOPTIMIZER_INSTALL_LICENSES			${MESHOPTIMIZER_LICENSE_MD} )
	set( MESHOPTIMIZER_INSTALL_DLLS_DEBUG 		${MESHOPTIMIZER_DEBUG_DLL} )
	set( MESHOPTIMIZER_INSTALL_DLLS_RELEASE 	${MESHOPTIMIZER_RELEASE_DLL} )
endif()