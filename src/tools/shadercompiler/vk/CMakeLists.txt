cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( VulkanSDK REQUIRED )

set( THIRDPARTY_INCLUDES		${VULKANSDK_INCLUDE} )
set( THIRDPARTY_LIBS			${VULKANSDK_SPIRVCROSS_LIBS} ${VULKANSDK_GLSLANG_LIBS} ${VULKANSDK_SPIRVTOOLS_LIBS} ${VULKANSDK_SHADERC_LIBS} )


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

filter_platform_files( SRC_FILES        ${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_SRC_FILES ${PLATFORM_SRC_DIR} )
filter_platform_files( GENERATED_FILES 	${PLATFORM_SRC_DIR} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                     								${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( shadercompiler_vk SHARED 	                      	    					${FULL_SRC_FILES} )
set_target_properties( shadercompiler_vk PROPERTIES FOLDER         						"Tools/Shader Compiler" )
set_target_properties( shadercompiler_vk PROPERTIES PREFIX 								"" )

target_include_directories( shadercompiler_vk PRIVATE             						${SHARED_SRC_DIR} ${THIRDPARTY_INCLUDES} )
target_link_libraries( shadercompiler_vk PRIVATE										core stdlib interfaces shadercache ${THIRDPARTY_LIBS} ${CMAKE_DL_LIBS} )
add_common_target_defines( shadercompiler_vk )


#
# Install
#
inherit_install_files( shadercompiler_vk								TARGETS core stdlib interfaces shadercache )

# Binaries
install_files( shadercompiler_vk TARGETS RUNTIME shadercompiler_vk 		DESTINATION bin/${PLATFORM_BIN_DIR}/ )

# Linceses
install_files( shadercompiler_vk FILES ${VULKANSDK_INSTALL_LICENSES}	DESTINATION licenses/vulkansdk/ )