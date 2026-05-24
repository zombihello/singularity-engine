cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( Mimalloc REQUIRED )

set( THIRDPARTY_INCLUDES					${MIMALLOC_INCLUDE} )
set( THIRDPARTY_DEFINES )
set( THIRDPARTY_LIBS						${MIMALLOC_LIBS} )
set( THIRDPARTY_INSTALL_TOOLS )
set( THIRDPARTY_INSTALL_DLLS )
set( THIRDPARTY_INSTALL_DEBUG_DLLS			${MIMALLOC_INSTALL_DLLS_DEBUG} )
set( THIRDPARTY_INSTALL_RELEASE_DLLS		${MIMALLOC_INSTALL_DLLS_RELEASE} )

# Third parties for specific platforms
if ( PLATFORM_WINDOWS )
	find_package( SDL3 REQUIRED )
	find_package( Tracy REQUIRED )
	find_package( BugTrap REQUIRED )

	set( THIRDPARTY_INCLUDES					${THIRDPARTY_INCLUDES} ${SDL3_INCLUDE} ${TRACY_INCLUDE} ${BUGTRAP_INCLUDE} )
	set( THIRDPARTY_DEFINES 					${THIRDPARTY_DEFINES} ${TRACY_DEFINES} )
	set( THIRDPARTY_LIBS						${THIRDPARTY_LIBS} ${SDL3_LIBS} ${TRACY_LIBS} ${BUGTRAP_LIBS} )
	set( THIRDPARTY_INSTALL_TOOLS 				${THIRDPARTY_INSTALL_TOOLS} ${TRACY_INSTALL_TOOLS} ${BUGTRAP_INSTALL_TOOLS} )
	set( THIRDPARTY_INSTALL_DLLS				${THIRDPARTY_INSTALL_DLLS} ${BUGTRAP_INSTALL_DLLS} )
	set( THIRDPARTY_INSTALL_DEBUG_DLLS			${THIRDPARTY_INSTALL_DEBUG_DLLS} ${SDL3_INSTALL_DLLS_DEBUG} ${TRACY_INSTALL_DLLS_DEBUG} ${BUGTRAP_INSTALL_DLLS_DEBUG} )
	set( THIRDPARTY_INSTALL_RELEASE_DLLS		${THIRDPARTY_INSTALL_RELEASE_DLLS} ${SDL3_INSTALL_DLLS_RELEASE} ${TRACY_INSTALL_DLLS_RELEASE} ${BUGTRAP_INSTALL_DLLS_RELEASE} )
endif()


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tier0/*.h" "${PUBLIC_SRC_DIR}/tier0/*.inl" "${PUBLIC_SRC_DIR}/tier0/*.cpp" )
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
add_library( tier0 SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( tier0 PRIVATE 							pch_tier0.h )
set_target_properties( tier0 PROPERTIES FOLDER         				Engine/Tiers )
set_target_properties( tier0 PROPERTIES PREFIX 						"" )

target_include_directories( tier0 PRIVATE             				${THIRDPARTY_INCLUDES} )
target_compile_definitions( tier0 PRIVATE							${THIRDPARTY_DEFINES} )
target_link_libraries( tier0 PRIVATE 								tier1 ${THIRDPARTY_LIBS} ${CMAKE_DL_LIBS} )
if ( PLATFORM_WINDOWS )
	target_link_libraries( tier0 PRIVATE 							dbghelp )
endif()
add_common_target_defines( tier0 )


#
# Install
#
inherit_install_files( tier0									TARGETS tier1 )

# Binaries
install_files( tier0 TARGETS RUNTIME tier0						DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( tier0 FILES $<TARGET_PDB_FILE:tier0>   		DESTINATION 	bin/${PLATFORM_BIN_DIR}/ )
endif()
install_files( tier0 FILES ${THIRDPARTY_INSTALL_DLLS}			DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_THIRDPARTY_TOOLS )
	install_files( tier0 FILES ${THIRDPARTY_INSTALL_TOOLS}		DESTINATION		bin/${PLATFORM_BIN_DIR}/ )
endif()
install_files( tier0 FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}		DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( tier0 FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}	DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( tier0 FILES ${MIMALLOC_INSTALL_LICENSES}			DESTINATION 	licenses/mimalloc/ )
if ( PLATFORM_WINDOWS )
	install_files( tier0 FILES ${SDL3_INSTALL_LICENSES}			DESTINATION 	licenses/sdl3/ )
	install_files( tier0 FILES ${TRACY_INSTALL_LICENSES}		DESTINATION 	licenses/tracy/ )
	install_files( tier0 FILES ${BUGTRAP_INSTALL_LICENSES}		DESTINATION 	licenses/bugtrap/ )
endif()