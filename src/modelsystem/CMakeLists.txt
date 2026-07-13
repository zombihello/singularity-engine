cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/modelsystem/*.h" "${PUBLIC_SRC_DIR}/modelsystem/*.inl" "${PUBLIC_SRC_DIR}/modelsystem/*.cpp"
										"${PUBLIC_SRC_DIR}/tier0/*.cpp" )
set( GENERATED_SRC_PATTERNS				"${GENERATED_DIR}/*.h" "${GENERATED_DIR}/*.inl" "${GENERATED_DIR}/*.cpp"
										"${GENERATED_DIR}/*.hxx" "${GENERATED_DIR}/*.cxx" )
set( PUBLIC_VIEW_ONLY_PATTERNS 			"${PUBLIC_SRC_DIR}/modelsystem/*.hlsl" "${PUBLIC_SRC_DIR}/modelsystem/*.list" )

if ( PLATFORM_WINDOWS )
	set( SRC_PATTERNS 					${SRC_PATTERNS} "${SRC_DIR}/*.rc" )
else()
	message( SEND_ERROR "Unknown platform"  )
endif()


file( GLOB_RECURSE SRC_FILES        			CONFIGURE_DEPENDS           ${SRC_PATTERNS} )
file( GLOB_RECURSE PUBLIC_SRC_FILES 			CONFIGURE_DEPENDS           ${PUBLIC_SRC_PATTERNS} )
file( GLOB_RECURSE GENERATED_FILES  			CONFIGURE_DEPENDS           ${GENERATED_SRC_PATTERNS} )
file( GLOB_RECURSE PUBLIC_VIEW_ONLY_FILES		CONFIGURE_DEPENDS           ${PUBLIC_VIEW_ONLY_PATTERNS} )

# Filter out all unnecessary platforms
filter_platform_files( SRC_FILES				${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_SRC_FILES			${PLATFORM_SRC_DIR} )
filter_platform_files( GENERATED_FILES 			${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_VIEW_ONLY_FILES 	${PLATFORM_SRC_DIR} )

# Just add the source files to the project as documents
set_source_files_properties( ${PUBLIC_VIEW_ONLY_FILES}     	PROPERTIES      VS_TOOL_OVERRIDE "None" ) 


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} ${PUBLIC_VIEW_ONLY_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                     					${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} ${PUBLIC_VIEW_ONLY_FILES} )
add_library( modelsystem SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( modelsystem PRIVATE 								pch_modelsystem.h )
set_target_properties( modelsystem PROPERTIES FOLDER         				Engine )
set_target_properties( modelsystem PROPERTIES PREFIX 						"" )

target_link_libraries( modelsystem PRIVATE									tier0 tier1 interfaces smdldoc ${CMAKE_DL_LIBS} )
add_common_target_defines( modelsystem )
add_dependencies( materialsystem											studiorender )


#
# Install
#
inherit_install_files( modelsystem										TARGETS tier0 tier1 interfaces smdldoc studiorender )
install_files( modelsystem TARGETS RUNTIME modelsystem 					DESTINATION bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( modelsystem FILES $<TARGET_PDB_FILE:modelsystem>     DESTINATION bin/${PLATFORM_BIN_DIR}/ )
endif()