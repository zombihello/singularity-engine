cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/schemasystem/*.h" "${PUBLIC_SRC_DIR}/schemasystem/*.inl" "${PUBLIC_SRC_DIR}/schemasystem/*.cpp" 
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
list( FILTER SRC_FILES 					EXCLUDE REGEX "/schemalib/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/schemalib/" )

filter_platform_files( SRC_FILES        ${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_SRC_FILES ${PLATFORM_SRC_DIR} )
filter_platform_files( GENERATED_FILES 	${PLATFORM_SRC_DIR} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                     		${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( schemasystem SHARED 	                      	    ${FULL_SRC_FILES} )
target_precompile_headers( schemasystem PRIVATE 				pch_schemasystem.h )
set_target_properties( schemasystem PROPERTIES FOLDER         	Engine )
set_target_properties( schemasystem PROPERTIES PREFIX 			"" )
target_link_libraries( schemasystem PRIVATE						tier0 tier1 interfaces ${CMAKE_DL_LIBS} )
add_common_target_defines( schemasystem )


#
# Install
#
inherit_install_files( schemasystem										TARGETS tier0 tier1 interfaces )

# Binaries
install_files( schemasystem TARGETS RUNTIME schemasystem 				DESTINATION	bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( schemasystem FILES $<TARGET_PDB_FILE:schemasystem>   DESTINATION bin/${PLATFORM_BIN_DIR}/ )
endif()