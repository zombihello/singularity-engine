cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/studiorender/*.h" "${PUBLIC_SRC_DIR}/studiorender/*.inl" "${PUBLIC_SRC_DIR}/studiorender/*.cpp" 
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
list( FILTER SRC_FILES 					EXCLUDE REGEX "/studioapi_vk/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/studioapi/" )

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
add_library( studiorender SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( studiorender PRIVATE 							pch_studiorender.h )
set_target_properties( studiorender PROPERTIES FOLDER         				Engine )
set_target_properties( studiorender PROPERTIES PREFIX 						"" )

target_link_libraries( studiorender PRIVATE									core stdlib interfaces shadercache ${CMAKE_DL_LIBS} )
add_common_target_defines( studiorender )
add_dependencies( studiorender												studioapi_vk )


#
# Install
#
inherit_install_files( studiorender								TARGETS core stdlib interfaces shadercache studioapi_vk )

# Binaries
install_files( studiorender TARGETS RUNTIME studiorender 		DESTINATION bin/${PLATFORM_BIN_DIR}/ )