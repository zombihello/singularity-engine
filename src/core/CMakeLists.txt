cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( Mimalloc REQUIRED )
find_package( Zlib REQUIRED )
find_package( SDL2 REQUIRED )
find_package( Optick REQUIRED )
find_package( BugTrap REQUIRED )

set( THIRDPARTY_INCLUDES					${MIMALLOC_INCLUDE} ${ZLIB_INCLUDE} ${SDL2_INCLUDE} ${OPTICK_INCLUDE} ${BUGTRAP_INCLUDE} )
set( THIRDPARTY_LIBS						${MIMALLOC_LIBS} ${ZLIB_LIBS} ${SDL2_LIBS} ${OPTICK_LIBS} ${BUGTRAP_LIBS} )
set( THIRDPARTY_INSTALL_TOOLS 				${OPTICK_INSTALL_TOOLS} ${BUGTRAP_INSTALL_TOOLS} )
set( THIRDPARTY_INSTALL_DLLS				${BUGTRAP_INSTALL_DLLS} )
set( THIRDPARTY_INSTALL_DEBUG_DLLS			${MIMALLOC_INSTALL_DLLS_DEBUG} ${ZLIB_INSTALL_DLLS_DEBUG} ${SDL2_INSTALL_DLLS_DEBUG} ${OPTICK_INSTALL_DLLS_DEBUG}
											${BUGTRAP_INSTALL_DLLS_DEBUG} )
set( THIRDPARTY_INSTALL_RELEASE_DLLS		${MIMALLOC_INSTALL_DLLS_RELEASE} ${ZLIB_INSTALL_DLLS_RELEASE} ${SDL2_INSTALL_DLLS_RELEASE} ${OPTICK_INSTALL_DLLS_RELEASE}
											${BUGTRAP_INSTALL_DLLS_RELEASE} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/core/*.h" "${PUBLIC_SRC_DIR}/core/*.inl" "${PUBLIC_SRC_DIR}/core/*.cpp" )
set( GENERATED_SRC_PATTERNS				"${GENERATED_DIR}/*.h" "${GENERATED_DIR}/*.inl" "${GENERATED_DIR}/*.cpp"
										"${GENERATED_DIR}/*.hxx" "${GENERATED_DIR}/*.cxx" )

if ( PLATFORM_WINDOWS )
	set( SRC_PATTERNS 					${SRC_PATTERNS} "${SRC_DIR}/*.rc" )
else()
	message( SEND_ERROR "Unknown platform"  )
endif()


file( GLOB_RECURSE SRC_FILES        CONFIGURE_DEPENDS           ${SRC_PATTERNS} )
file( GLOB_RECURSE PUBLIC_SRC_FILES CONFIGURE_DEPENDS           ${PUBLIC_SRC_PATTERNS} )
file( GLOB_RECURSE GENERATED_FILES  CONFIGURE_DEPENDS           ${GENERATED_SRC_PATTERNS} )

# Filter out all unnecessary platforms
filter_platform_files( SRC_FILES        ${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_SRC_FILES ${PLATFORM_SRC_DIR} )
filter_platform_files( GENERATED_FILES 	${PLATFORM_SRC_DIR} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                     			${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( core SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( core PRIVATE 							pch_core.h )
set_target_properties( core PROPERTIES FOLDER         				Engine )
set_target_properties( core PROPERTIES PREFIX 						"" )

target_include_directories( core PRIVATE             				${THIRDPARTY_INCLUDES} )
target_link_libraries( core PRIVATE 								stdlib ${THIRDPARTY_LIBS} ${CMAKE_DL_LIBS} )
add_common_target_defines( core )


#
# Install
#
inherit_install_files( core										TARGETS stdlib )

# Binaries
install_files( core TARGETS RUNTIME core						DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
install_files( core FILES ${THIRDPARTY_INSTALL_DLLS}			DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_THIRDPARTY_TOOLS )
	install_files( core FILES ${THIRDPARTY_INSTALL_TOOLS}		DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
endif()
install_files( core FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}		DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( core FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}	DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( core FILES ${MIMALLOC_INSTALL_LICENSES}			DESTINATION 	licenses/mimalloc/ )
install_files( core FILES ${ZLIB_INSTALL_LICENSES}				DESTINATION 	licenses/zlib/ )
install_files( core FILES ${SDL2_INSTALL_LICENSES}				DESTINATION 	licenses/sdl2/ )
install_files( core FILES ${OPTICK_INSTALL_LICENSES}			DESTINATION 	licenses/optick/ )
install_files( core FILES ${BUGTRAP_INSTALL_LICENSES}			DESTINATION 	licenses/bugtrap/ )