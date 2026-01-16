# ---------------------------------
#	[in] 	ASSIMP_PATH						- Root directory Assimp
#	[out] 	ASSIMP_INCLUDE					- Directory with includes
#	[out]	ASSIMP_LIBS						- Libs Assimp to link
# 	[out] 	ASSIMP_INSTALL_LICENSES			- Assimp licenses to install
# 	[out] 	ASSIMP_INSTALL_DLLS_DEBUG 		- Assimp DLLs to install (Debug)
# 	[out] 	ASSIMP_INSTALL_DLLS_RELEASE 	- Assimp DLLs to install (Release)
#	[out]	ASSIMP_FOUND					- Is found Assimp
# ---------------------------------

set( ASSIMP_FOUND 			OFF )
set( ASSIMP_SEARCH_PATHS 	${ASSIMP_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( ASSIMP_LIB_PATH_SUFFIXES		"lib/win64" )
		set( ASSIMP_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( ASSIMP_LIB_PATH_SUFFIXES		"lib/win32" )
		set( ASSIMP_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Assimp :: Unknown bit platform" )
		return()
	endif()

	set( ASSIMP_DEBUG_LIB_NAMES 	"assimpd" )
	set( ASSIMP_DEBUG_DLL_NAMES 	"assimpd.dll" )
	set( ASSIMP_RELEASE_LIB_NAMES 	"assimp" )
	set( ASSIMP_RELEASE_DLL_NAMES 	"assimp.dll" )

# Unknown platform
else()
	message( SEND_ERROR "Assimp :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		ASSIMP_INCLUDE
				NO_DEFAULT_PATH
				NAMES "assimp/config.h"
				PATH_SUFFIXES include
				PATHS ${ASSIMP_SEARCH_PATHS} )

# Libs
find_library( 	ASSIMP_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${ASSIMP_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${ASSIMP_LIB_PATH_SUFFIXES}/debug
				PATHS ${ASSIMP_SEARCH_PATHS} )
find_library( 	ASSIMP_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${ASSIMP_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${ASSIMP_LIB_PATH_SUFFIXES}/release
				PATHS ${ASSIMP_SEARCH_PATHS} )

# DLLs
find_file( 		ASSIMP_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${ASSIMP_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${ASSIMP_BIN_PATH_SUFFIXES}/debug
                PATHS ${ASSIMP_SEARCH_PATHS} )
find_file( 		ASSIMP_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${ASSIMP_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${ASSIMP_BIN_PATH_SUFFIXES}/release
                PATHS ${ASSIMP_SEARCH_PATHS} )

# Licenses
find_file( 		ASSIMP_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${ASSIMP_SEARCH_PATHS} )


if ( NOT ASSIMP_INCLUDE OR NOT ASSIMP_LIB_DEBUG OR NOT ASSIMP_LIB_RELEASE OR NOT ASSIMP_DEBUG_DLL OR NOT ASSIMP_RELEASE_DLL )
	message( SEND_ERROR "Assimp :: Not found" )
	return()
else()
	set( ASSIMP_FOUND 					ON )
	set( ASSIMP_LIBS 					$<$<CONFIG:Debug>:${ASSIMP_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${ASSIMP_LIB_RELEASE}> )
	set( ASSIMP_INSTALL_LICENSES		${ASSIMP_LICENSE} )
	set( ASSIMP_INSTALL_DLLS_DEBUG 		${ASSIMP_DEBUG_DLL} )
	set( ASSIMP_INSTALL_DLLS_RELEASE 	${ASSIMP_RELEASE_DLL} )
endif()
