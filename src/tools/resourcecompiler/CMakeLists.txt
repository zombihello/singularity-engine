cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tools/resource_tools/*.h" "${PUBLIC_SRC_DIR}/tools/resource_tools/*.inl" "${PUBLIC_SRC_DIR}/tools/resource_tools/*.cpp" 
										"${PUBLIC_SRC_DIR}/tier0/*.cpp" )
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


set( FULL_SRC_FILES                                     						${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_executable( resourcecompiler	                      	    				${FULL_SRC_FILES} )
target_precompile_headers( resourcecompiler PRIVATE 							pch_resourcecompiler.h )
set_target_properties( resourcecompiler PROPERTIES FOLDER         				"Tools/Resource Compiler" )
set_target_properties( resourcecompiler PROPERTIES PREFIX 						"" )

target_link_libraries( resourcecompiler PRIVATE									tier0 tier1 appframework interfaces stexdoc smatdoc smdldoc sentdoc smapdoc ${CMAKE_DL_LIBS} )
add_common_target_defines( resourcecompiler )
add_dependencies( resourcecompiler 												filesystem cvar texture_tool material_tool model_tool entitydesc_tool map_tool )


#
# Install
#
inherit_install_files( resourcecompiler									TARGETS tier0 filesystem cvar texture_tool material_tool model_tool entitydesc_tool map_tool tier1 appframework interfaces stexdoc smatdoc smdldoc sentdoc smapdoc )

# Binaries
install_files( resourcecompiler TARGETS RUNTIME resourcecompiler 		DESTINATION     bin/${PLATFORM_BIN_DIR}/ )

# Setup install the target
if ( INSTALL_SDK_TOOLS )
	install_target( resourcecompiler									DESTINATION		game )
endif()