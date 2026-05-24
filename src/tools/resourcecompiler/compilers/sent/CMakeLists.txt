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
add_library( resourcecompiler_sent SHARED 	                      	    		${FULL_SRC_FILES} )
set_target_properties( resourcecompiler_sent PROPERTIES FOLDER         			"Tools/Resource Compiler" )

target_link_libraries( resourcecompiler_sent PRIVATE							tier0 tier1 interfaces sentdoc ${CMAKE_DL_LIBS} )
add_common_target_defines( resourcecompiler_sent )


#
# Install
#
inherit_install_files( resourcecompiler_sent												TARGETS tier0 tier1 interfaces sentdoc )

# Binaries
install_files( resourcecompiler_sent TARGETS RUNTIME resourcecompiler_sent 					DESTINATION bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( resourcecompiler_sent FILES $<TARGET_PDB_FILE:resourcecompiler_sent>   	DESTINATION bin/${PLATFORM_BIN_DIR}/ )
endif()