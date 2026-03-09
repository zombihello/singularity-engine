# ---------------------------------
#	[in] 	BUGTRAP_PATH					- Root directory BugTrap
#	[out] 	BUGTRAP_INCLUDE					- Directory with includes
#	[out]	BUGTRAP_LIBS					- Libs BugTrap to link
# 	[out] 	BUGTRAP_INSTALL_LICENSES		- BugTrap licenses to install
# 	[out] 	BUGTRAP_INSTALL_DLLS_DEBUG 		- BugTrap DLLs to install (Debug)
# 	[out] 	BUGTRAP_INSTALL_DLLS_RELEASE 	- BugTrap DLLs to install (Release)
#	[out] 	BUGTRAP_INSTALL_DLLS 			- BugTrap DLLs to install (Configuration independent)
#	[out] 	BUGTRAP_INSTALL_TOOLS 			- BugTrap tools to install
#	[out]	BUGTRAP_FOUND					- Is found BugTrap
# ---------------------------------

set( BUGTRAP_FOUND 			OFF )
set( BUGTRAP_SEARCH_PATHS 	${BUGTRAP_PATH} )


#
# Set path suffixed to libs for current platform
#
# Windows
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( BUGTRAP_LIB_PATH_SUFFIXES		"lib/win64" )
		set( BUGTRAP_BIN_PATH_SUFFIXES		"bin/win64" )
	elseif( PLATFORM_32BIT )
		set( BUGTRAP_LIB_PATH_SUFFIXES		"lib/win32" )
		set( BUGTRAP_BIN_PATH_SUFFIXES		"bin/win32" )
	else()
		message( SEND_ERROR "BugTrap :: Unknown bit platform" )
		return()
	endif()

	set( BUGTRAP_DEBUG_LIB_NAMES 			"BugTrapD-x64" )
	set( BUGTRAP_DEBUG_DLL_NAMES 			"BugTrapD-x64.dll" )
	set( BUGTRAP_RELEASE_LIB_NAMES 			"BugTrap-x64" )
	set( BUGTRAP_RELEASE_DLL_NAMES 			"BugTrap-x64.dll" )
	set( BUGTRAP_CRASH_EXPLORER_EXE_NAMES 	"CrashExplorer.exe" )
	set( BUGTRAP_DBGHELP_DLL_NAMES			"dbghelp.dll" )

# Unknown platform
else()
	message( SEND_ERROR "BugTrap :: Unknown platform" )
	return()
endif()


#
# Find pathes
#
# Includes
find_path( 		BUGTRAP_INCLUDE
				NO_DEFAULT_PATH
				NAMES "BugTrap.h"
				PATH_SUFFIXES include
				PATHS ${BUGTRAP_SEARCH_PATHS} )

# Libs
find_library( 	BUGTRAP_LIB_DEBUG
				NO_DEFAULT_PATH
                NAMES ${BUGTRAP_DEBUG_LIB_NAMES}
				PATH_SUFFIXES ${BUGTRAP_LIB_PATH_SUFFIXES}/debug
				PATHS ${BUGTRAP_SEARCH_PATHS} )
find_library( 	BUGTRAP_LIB_RELEASE
				NO_DEFAULT_PATH
                NAMES ${BUGTRAP_RELEASE_LIB_NAMES}
				PATH_SUFFIXES ${BUGTRAP_LIB_PATH_SUFFIXES}/release
				PATHS ${BUGTRAP_SEARCH_PATHS} )

# DLLs
find_file( 		BUGTRAP_DEBUG_DLL
				NO_DEFAULT_PATH
                NAMES ${BUGTRAP_DEBUG_DLL_NAMES}
				PATH_SUFFIXES ${BUGTRAP_BIN_PATH_SUFFIXES}/debug
                PATHS ${BUGTRAP_SEARCH_PATHS} )
find_file( 		BUGTRAP_RELEASE_DLL
				NO_DEFAULT_PATH
                NAMES ${BUGTRAP_RELEASE_DLL_NAMES}
				PATH_SUFFIXES ${BUGTRAP_BIN_PATH_SUFFIXES}/release
                PATHS ${BUGTRAP_SEARCH_PATHS} )
find_file( 		BUGTRAP_DBGHELP_DLL
				NO_DEFAULT_PATH
				NAMES ${BUGTRAP_DBGHELP_DLL_NAMES}
				PATH_SUFFIXES ${BUGTRAP_BIN_PATH_SUFFIXES}
				PATHS ${BUGTRAP_SEARCH_PATHS} )

# Exes
find_file( 		BUGTRAP_CRASH_EXPLORER_EXE
				NO_DEFAULT_PATH
				NAMES ${BUGTRAP_CRASH_EXPLORER_EXE_NAMES}
				PATH_SUFFIXES ${BUGTRAP_BIN_PATH_SUFFIXES}
				PATHS ${BUGTRAP_SEARCH_PATHS} )

# Licenses
find_file( 		BUGTRAP_LICENSE
				NO_DEFAULT_PATH
				NAMES "LICENSE"
				PATHS ${BUGTRAP_SEARCH_PATHS} )


if ( NOT BUGTRAP_INCLUDE OR NOT BUGTRAP_LIB_DEBUG OR NOT BUGTRAP_LIB_RELEASE OR NOT BUGTRAP_DEBUG_DLL OR NOT BUGTRAP_RELEASE_DLL OR NOT BUGTRAP_DBGHELP_DLL OR NOT BUGTRAP_CRASH_EXPLORER_EXE )
	message( SEND_ERROR "BugTrap :: Not found" )
	return()
else()
	set( BUGTRAP_FOUND 					ON )
	set( BUGTRAP_LIBS 					$<$<CONFIG:Debug>:${BUGTRAP_LIB_DEBUG}> $<$<CONFIG:Release,Retail>:${BUGTRAP_LIB_RELEASE}> )
	set( BUGTRAP_INSTALL_LICENSES		${BUGTRAP_LICENSE} )
	set( BUGTRAP_INSTALL_DLLS_DEBUG 	${BUGTRAP_DEBUG_DLL} )
	set( BUGTRAP_INSTALL_DLLS_RELEASE 	${BUGTRAP_RELEASE_DLL} )
	set( BUGTRAP_INSTALL_DLLS 			${BUGTRAP_DBGHELP_DLL} )
	set( BUGTRAP_INSTALL_TOOLS 			$<$<CONFIG:Debug,Release>:${BUGTRAP_CRASH_EXPLORER_EXE}> )
endif()
