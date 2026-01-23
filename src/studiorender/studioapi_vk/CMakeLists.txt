cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( VulkanSDK REQUIRED )

set( THIRDPARTY_INCLUDES		${VULKANSDK_INCLUDE} )
set( THIRDPARTY_DEFINES 		${VULKANSDK_VOLK_DEFINES} )
set( THIRDPARTY_LIBS			${VULKANSDK_VOLK_LIBS} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/studiorender/studioapi/*.h" "${PUBLIC_SRC_DIR}/studiorender/studioapi/*.inl" "${PUBLIC_SRC_DIR}/studiorender/studioapi/*.cpp" 
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


set( FULL_SRC_FILES                                     					${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( studioapi_vk SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( studioapi_vk PRIVATE 							pch_studioapi_vk.h )
set_target_properties( studioapi_vk PROPERTIES FOLDER         				Engine )
set_target_properties( studioapi_vk PROPERTIES PREFIX 						"" )

target_include_directories( studioapi_vk PRIVATE             				${SHARED_SRC_DIR} ${THIRDPARTY_INCLUDES} )
target_compile_definitions( studioapi_vk PRIVATE							VK_NO_PROTOTYPES ${THIRDPARTY_DEFINES} )
target_link_libraries( studioapi_vk PRIVATE									core stdlib interfaces shadercache pixelformatinfos ${THIRDPARTY_LIBS} ${CMAKE_DL_LIBS} )
add_common_target_defines( studioapi_vk )


#
# Install
#
inherit_install_files( studioapi_vk									TARGETS core stdlib interfaces shadercache pixelformatinfos )

# Binaries
install_files( studioapi_vk TARGETS RUNTIME studioapi_vk 			DESTINATION bin/${PLATFORM_BIN_DIR}/ )

# Linceses
install_files( studioapi_vk FILES ${VULKANSDK_INSTALL_LICENSES}		DESTINATION licenses/vulkansdk/ )