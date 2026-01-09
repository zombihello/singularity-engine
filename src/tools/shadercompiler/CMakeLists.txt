cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( RapidJson REQUIRED )
find_package( GLM REQUIRED )

set( THIRDPARTY_INCLUDES				${RAPIDJSON_INCLUDE} ${GLM_INCLUDE} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tools/shadercompiler/*.h" "${PUBLIC_SRC_DIR}/tools/shadercompiler/*.inl" "${PUBLIC_SRC_DIR}/tools/shadercompiler/*.cpp" 
										"${PUBLIC_SRC_DIR}/core/*.cpp" )
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

# Filter out all unnecessary files
list( FILTER SRC_FILES 					EXCLUDE REGEX "/vk/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/vk/" )

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


set( FULL_SRC_FILES                                     					${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_executable( shadercompiler	                      	    				${FULL_SRC_FILES} )
target_precompile_headers( shadercompiler PRIVATE 							pch_shadercompiler.h )
set_target_properties( shadercompiler PROPERTIES FOLDER         			"Tools/Shader Compiler" )
set_target_properties( shadercompiler PROPERTIES PREFIX 					"" )

target_include_directories( shadercompiler PRIVATE             				${THIRDPARTY_INCLUDES} )
target_link_libraries( shadercompiler 										core stdlib appframework interfaces shadercache ${CMAKE_DL_LIBS} )
add_common_target_defines( shadercompiler )
add_dependencies( shadercompiler 											filesystem cvar shadercompiler_vk )


#
# Install
#
inherit_install_files( shadercompiler								core stdlib appframework interfaces shadercache filesystem cvar shadercompiler_vk )

# Binaries
install_files( shadercompiler TARGETS RUNTIME shadercompiler 		DESTINATION     bin/${PLATFORM_BIN_DIR}/ )

# Linceses
install_files( shadercompiler FILES ${RAPIDJSON_INSTALL_LICENSES} 	DESTINATION 	licenses/rapidjson/ )
install_files( shadercompiler FILES ${GLM_INSTALL_LICENSES} 		DESTINATION 	licenses/glm/ )

# Setup install the target
if ( INSTALL_TOOLS )
	install_target( shadercompiler									DESTINATION		src/devtools/shadercompiler/ )
endif()