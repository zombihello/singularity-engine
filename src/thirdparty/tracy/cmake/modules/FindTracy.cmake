# ---------------------------------
#	[in] 	TRACY_PATH						- Root directory Tracy
#	[out] 	TRACY_INCLUDE					- Directory with includes
#	[out]	TRACY_DEFINES					- Tracy defines to set
#	[out]	TRACY_LIBS						- Libs Tracy to link
# 	[out] 	TRACY_INSTALL_LICENSES			- Tracy licenses to install
# 	[out] 	TRACY_INSTALL_DLLS_DEBUG 		- Tracy DLLs to install (Debug)
# 	[out] 	TRACY_INSTALL_DLLS_RELEASE 		- Tracy DLLs to install (Release)
#	[out] 	TRACY_INSTALL_TOOLS 			- Tracy tools to install
#	[out]	TRACY_FOUND						- Is found Tracy
# ---------------------------------

set( TRACY_FOUND 				OFF )
set( TRACY_SEARCH_PATHS 		${TRACY_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( TRACY_LIB_PATH_SUFFIXES		"lib/win64" )
		set( TRACY_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( TRACY_LIB_PATH_SUFFIXES		"lib/win32" )
		set( TRACY_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "Tracy :: Unknown bit platform" )
		return()
	endif()

	set( TRACY_DEBUG_LIB_NAMES 				"TracyClientd" )
	set( TRACY_DEBUG_DLL_NAMES 				"TracyClientd.dll" )
	set( TRACY_RELEASE_LIB_NAMES 			"TracyClient" )
	set( TRACY_RELEASE_DLL_NAMES 			"TracyClient.dll" )
	set( TRACY_CAPTURE_EXE_NAMES 			"tracy-capture.exe" )
	set( TRACY_CSVEXPORT_EXE_NAMES 			"tracy-csvexport.exe" )
	set( TRACY_IMPORT_CHROME_EXE_NAMES 		"tracy-import-chrome.exe" )
	set( TRACY_IMPORT_FUCHSIA_EXE_NAMES 	"tracy-import-fuchsia.exe" )
	set( TRACY_PROFILER_EXE_NAMES 			"tracy-profiler.exe" )
	set( TRACY_UPDATE_EXE_NAMES 			"tracy-update.exe" )

# Unknown platform
else()
	message( SEND_ERROR "Tracy :: Unknown platform" )
	return()
endif()


# Set Tracy defines
set( TRACY_DEFINES 		TRACY_ENABLE TRACY_IMPORT TRACY_ON_DEMAND TRACY_NO_CRASH_HANDLER )


#
# Find pathes
#
# Includes
find_path( 		TRACY_INCLUDE
				NO_DEFAULT_PATH
				NAMES "tracy/tracy/Tracy.hpp"
				PATH_SUFFIXES include
				PATHS ${TRACY_SEARCH_PATHS} )

# Libs
find_library( 	TRACY_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${TRACY_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${TRACY_LIB_PATH_SUFFIXES}/debug
                PATHS ${TRACY_SEARCH_PATHS} )
find_library( 	TRACY_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${TRACY_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${TRACY_LIB_PATH_SUFFIXES}/release
                PATHS ${TRACY_SEARCH_PATHS} )

# DLLs
find_file( 		TRACY_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${TRACY_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}/debug
                PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${TRACY_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}/release
                PATHS ${TRACY_SEARCH_PATHS} )

# Exes
find_file( 		TRACY_CAPTURE_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_CAPTURE_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_CSVEXPORT_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_CSVEXPORT_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_IMPORT_CHROME_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_IMPORT_CHROME_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_IMPORT_FUCHSIA_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_IMPORT_FUCHSIA_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_PROFILER_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_PROFILER_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )
find_file( 		TRACY_UPDATE_EXE
				NO_DEFAULT_PATH
				NAMES ${TRACY_UPDATE_EXE_NAMES}
				PATH_SUFFIXES ${TRACY_BIN_PATH_SUFFIXES}
				PATHS ${TRACY_SEARCH_PATHS} )

# Licenses
find_file( 		TRACY_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${TRACY_SEARCH_PATHS} )


if ( NOT TRACY_INCLUDE OR NOT TRACY_LIB_DEBUG OR NOT TRACY_LIB_RELEASE OR NOT TRACY_DEBUG_DLL OR NOT TRACY_RELEASE_DLL 
	OR NOT TRACY_CAPTURE_EXE OR NOT TRACY_CSVEXPORT_EXE OR NOT TRACY_IMPORT_CHROME_EXE OR NOT TRACY_IMPORT_FUCHSIA_EXE
	OR NOT TRACY_PROFILER_EXE OR NOT TRACY_UPDATE_EXE )
	message( SEND_ERROR "Tracy :: Not found" )
	return()
else()
	set( TRACY_FOUND 					ON )
	set( TRACY_LIBS						$<$<CONFIG:Debug>:${TRACY_LIB_DEBUG}> $<$<CONFIG:Release>:${TRACY_LIB_RELEASE}> )
	set( TRACY_INSTALL_LICENSES			$<$<CONFIG:Debug,Release>:${TRACY_LICENSE}> )
	set( TRACY_INSTALL_DLLS_DEBUG 		$<$<CONFIG:Debug>:${TRACY_DEBUG_DLL}> )
	set( TRACY_INSTALL_DLLS_RELEASE 	$<$<CONFIG:Release>:${TRACY_RELEASE_DLL}> )
	set( TRACY_INSTALL_TOOLS 			$<$<CONFIG:Debug,Release>:${TRACY_CAPTURE_EXE}> $<$<CONFIG:Debug,Release>:${TRACY_CSVEXPORT_EXE}> $<$<CONFIG:Debug,Release>:${TRACY_IMPORT_CHROME_EXE}> $<$<CONFIG:Debug,Release>:${TRACY_IMPORT_FUCHSIA_EXE}> $<$<CONFIG:Debug,Release>:${TRACY_PROFILER_EXE}> $<$<CONFIG:Debug,Release>:${TRACY_UPDATE_EXE}> )
endif()