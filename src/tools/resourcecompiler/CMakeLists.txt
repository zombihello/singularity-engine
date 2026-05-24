cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tools/resourcecompiler/*.h" "${PUBLIC_SRC_DIR}/tools/resourcecompiler/*.inl" "${PUBLIC_SRC_DIR}/tools/resourcecompiler/*.cpp" 
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

# Filter out all unnecessary files
list( FILTER SRC_FILES 					EXCLUDE REGEX "/compilers/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/compilers/" )

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

target_link_libraries( resourcecompiler PRIVATE									tier0 tier1 appframework interfaces gameinfo toolsinfo stexdoc smatdoc smdldoc sentdoc smapdoc ${CMAKE_DL_LIBS} )
add_common_target_defines( resourcecompiler )
add_dependencies( resourcecompiler 												filesystem cvar resourcecompiler_stex resourcecompiler_smat resourcecompiler_smdl resourcecompiler_sent resourcecompiler_smap )


#
# Install
#
inherit_install_files( resourcecompiler												TARGETS tier0 filesystem cvar resourcecompiler_stex resourcecompiler_smat resourcecompiler_smdl resourcecompiler_sent resourcecompiler_smap tier1 appframework interfaces gameinfo toolsinfo stexdoc smatdoc smdldoc sentdoc smapdoc )

# Binaries
install_files( resourcecompiler TARGETS RUNTIME resourcecompiler 					DESTINATION bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( resourcecompiler FILES $<TARGET_PDB_FILE:resourcecompiler>   	DESTINATION bin/${PLATFORM_BIN_DIR}/ )
endif()

# Setup install the target
if ( INSTALL_SDK_TOOLS )
	install_target( resourcecompiler												DESTINATION game )
endif()