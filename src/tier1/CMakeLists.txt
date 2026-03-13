cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( GLM REQUIRED )
find_package( EASTL REQUIRED )
find_package( Zlib REQUIRED )

set( THIRDPARTY_INCLUDES					${GLM_INCLUDE} ${EASTL_INCLUDE} ${ZLIB_INCLUDE} )
set( THIRDPARTY_DEFINES						${GLM_DEFINES} )
set( THIRDPARTY_LIBS						${EASTL_LIBS} ${ZLIB_LIBS} )
set( THIRDPARTY_INSTALL_DEBUG_DLLS			${ZLIB_INSTALL_DLLS_DEBUG} )
set( THIRDPARTY_INSTALL_RELEASE_DLLS		${ZLIB_INSTALL_DLLS_RELEASE} )
set( THIRDPARTY_VISUALIZERS					${GLM_VISUALIZERS} ${EASTL_VISUALIZERS} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tier1/*.h" "${PUBLIC_SRC_DIR}/tier1/*.inl" "${PUBLIC_SRC_DIR}/tier1/*.cpp" )
set( GENERATED_SRC_PATTERNS				"${GENERATED_DIR}/*.h" "${GENERATED_DIR}/*.inl" "${GENERATED_DIR}/*.cpp"
										"${GENERATED_DIR}/*.hxx" "${GENERATED_DIR}/*.cxx" )


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
source_group( "visualizers"										FILES	${THIRDPARTY_VISUALIZERS} )
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                         ${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} ${THIRDPARTY_VISUALIZERS} )
add_library( tier1 STATIC 	                      	        ${FULL_SRC_FILES} )
target_precompile_headers( tier1 PRIVATE 				    pch_tier1.h )
set_target_properties( tier1 PROPERTIES FOLDER				Engine/Tiers )
set_target_properties( tier1 PROPERTIES PREFIX 				"" )

target_include_directories( tier1 PUBLIC					${THIRDPARTY_INCLUDES} )
target_compile_definitions( tier1 PUBLIC					${THIRDPARTY_DEFINES} )
target_link_libraries( tier1 PUBLIC 						interfaces ${THIRDPARTY_LIBS} )
add_common_target_defines( tier1 )


#
# Install
#
inherit_install_files( tier1									TARGETS interfaces )

# Binaries
install_files( tier1 FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}		DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( tier1 FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}	DESTINATION		bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( tier1 FILES ${GLM_INSTALL_LICENSES}				DESTINATION 	licenses/glm/ )
install_files( tier1 FILES ${EASTL_INSTALL_LICENSES}			DESTINATION 	licenses/eastl/ )
install_files( tier1 FILES ${ZLIB_INSTALL_LICENSES}				DESTINATION 	licenses/zlib/ )