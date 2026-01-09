# ---------------------------------
#	[in] 	OPTICK_PATH						- Root directory Optick
#	[out] 	OPTICK_INCLUDE					- Directory with includes
#	[out]	OPTICK_LIBS						- Libs Optick to link
# 	[out] 	OPTICK_INSTALL_LICENSES			- Optick licenses to install
# 	[out] 	OPTICK_INSTALL_DLLS_DEBUG 		- Optick DLLs to install (Debug)
# 	[out] 	OPTICK_INSTALL_DLLS_RELEASE 	- Optick DLLs to install (Release)
#	[out] 	OPTICK_INSTALL_TOOLS 			- Optick tools to install
#	[out]	OPTICK_FOUND					- Is found Optick
# ---------------------------------

set( OPTICK_FOUND 			OFF )
set( OPTICK_SEARCH_PATHS 	${OPTICK_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( OPTICK_LIB_PATH_SUFFIXES		"lib/win64" )
		set( OPTICK_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( OPTICK_LIB_PATH_SUFFIXES		"lib/win32" )
		set( OPTICK_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Optick :: Unknown bit platform" )
		return()
	endif()

	set( OPTICK_DEBUG_LIB_NAMES 		"OptickCored" )
	set( OPTICK_DEBUG_DLL_NAMES 		"OptickCored.dll" )
	set( OPTICK_RELEASE_LIB_NAMES 		"OptickCore" )
	set( OPTICK_RELEASE_DLL_NAMES 		"OptickCore.dll" )
	set( OPTICK_OPTICK_EXE_NAMES 		"Optick.exe" )

# Unknown platform
else()
	message( SEND_ERROR "Optick :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		OPTICK_INCLUDE
				NO_DEFAULT_PATH
				NAMES "optick.h"
				PATH_SUFFIXES include
				PATHS ${OPTICK_SEARCH_PATHS} )

# Libs
find_library( 	OPTICK_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${OPTICK_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${OPTICK_LIB_PATH_SUFFIXES}/debug
				PATHS ${OPTICK_SEARCH_PATHS} )
find_library( 	OPTICK_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${OPTICK_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${OPTICK_LIB_PATH_SUFFIXES}/release
				PATHS ${OPTICK_SEARCH_PATHS} )

# DLLs
find_file( 		OPTICK_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${OPTICK_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${OPTICK_BIN_PATH_SUFFIXES}/debug
                PATHS ${OPTICK_SEARCH_PATHS} )
find_file( 		OPTICK_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${OPTICK_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${OPTICK_BIN_PATH_SUFFIXES}/release
                PATHS ${OPTICK_SEARCH_PATHS} )

# Exes
find_file( 		OPTICK_OPTICK_EXE
				NO_DEFAULT_PATH
				NAMES ${OPTICK_OPTICK_EXE_NAMES}
				PATH_SUFFIXES ${OPTICK_BIN_PATH_SUFFIXES}
				PATHS ${OPTICK_SEARCH_PATHS} )

# Licenses
find_file( 		OPTICK_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${OPTICK_SEARCH_PATHS} )


if ( NOT OPTICK_INCLUDE OR NOT OPTICK_LIB_DEBUG OR NOT OPTICK_LIB_RELEASE OR NOT OPTICK_DEBUG_DLL OR NOT OPTICK_RELEASE_DLL OR NOT OPTICK_OPTICK_EXE )
	message( SEND_ERROR "Optick :: Not found" )
	return()
else()
	set( OPTICK_FOUND 					ON )
	set( OPTICK_LIBS 					$<$<CONFIG:Debug>:${OPTICK_LIB_DEBUG}> $<$<CONFIG:Release>:${OPTICK_LIB_RELEASE}> )
	set( OPTICK_INSTALL_LICENSES		$<$<CONFIG:Debug,Release>:${OPTICK_LICENSE}> )
	set( OPTICK_INSTALL_DLLS_DEBUG 		${OPTICK_DEBUG_DLL} )
	set( OPTICK_INSTALL_DLLS_RELEASE 	$<$<CONFIG:Release>:${OPTICK_RELEASE_DLL}> )
	set( OPTICK_INSTALL_TOOLS 			$<$<CONFIG:Debug,Release>:${OPTICK_OPTICK_EXE}> )
endif()
