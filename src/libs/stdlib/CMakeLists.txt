cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( RapidJson REQUIRED )
find_package( GLM REQUIRED )

set( THIRDPARTY_INCLUDES		${RAPIDJSON_INCLUDE} ${GLM_INCLUDE} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/libs/stdlib/*.h" "${PUBLIC_SRC_DIR}/libs/stdlib/*.inl" "${PUBLIC_SRC_DIR}/libs/stdlib/*.cpp" )
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
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                         ${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( stdlib STATIC 	                      	        ${FULL_SRC_FILES} )
target_precompile_headers( stdlib PRIVATE 				    pch_stdlib.h )
set_target_properties( stdlib PROPERTIES FOLDER             Engine/Libraries )
set_target_properties( stdlib PROPERTIES PREFIX 		    "" )

target_include_directories( stdlib PRIVATE                  ${THIRDPARTY_INCLUDES} )
add_common_target_defines( stdlib )


#
# Install
#
# Linceses
install_files( stdlib FILES ${RAPIDJSON_INSTALL_LICENSES}    DESTINATION 	licenses/rapidjson/ )
install_files( stdlib FILES ${GLM_INSTALL_LICENSES}          DESTINATION 	licenses/glm/ )