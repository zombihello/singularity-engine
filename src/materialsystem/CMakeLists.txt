cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.shader" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/materialsystem/*.h" "${PUBLIC_SRC_DIR}/materialsystem/*.inl" "${PUBLIC_SRC_DIR}/materialsystem/*.cpp" 
										"${PUBLIC_SRC_DIR}/tier0/*.cpp" )
set( GENERATED_SRC_PATTERNS				"${GENERATED_DIR}/*.h" "${GENERATED_DIR}/*.inl" "${GENERATED_DIR}/*.cpp"
										"${GENERATED_DIR}/*.hxx" "${GENERATED_DIR}/*.cxx" )

set( VIEW_ONLY_PATTERNS					"${SRC_DIR}/*.makefile" "${SRC_DIR}/*.hlsl" )
set( PUBLIC_VIEW_ONLY_PATTERNS 			"${PUBLIC_SRC_DIR}/materialsystem/*.hlsl" )

if ( PLATFORM_WINDOWS )
	set( SRC_PATTERNS 					${SRC_PATTERNS} "${SRC_DIR}/*.rc" )
else()
	message( SEND_ERROR "Unknown platform"  )
endif()


file( GLOB_RECURSE SRC_FILES        				CONFIGURE_DEPENDS           ${SRC_PATTERNS} )
file( GLOB_RECURSE PUBLIC_SRC_FILES 				CONFIGURE_DEPENDS           ${PUBLIC_SRC_PATTERNS} )
file( GLOB_RECURSE GENERATED_FILES  				CONFIGURE_DEPENDS           ${GENERATED_SRC_PATTERNS} )
file( GLOB_RECURSE VIEW_ONLY_FILES        			CONFIGURE_DEPENDS           ${VIEW_ONLY_PATTERNS} )
file( GLOB_RECURSE PUBLIC_VIEW_ONLY_FILES        	CONFIGURE_DEPENDS           ${PUBLIC_VIEW_ONLY_PATTERNS} )

# Filter out all unnecessary files
list( FILTER SRC_FILES 					EXCLUDE REGEX "/stdshaders/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/stdshaders/" )
list( FILTER VIEW_ONLY_FILES 			EXCLUDE REGEX "/stdshaders/" )
list( FILTER PUBLIC_VIEW_ONLY_FILES 	EXCLUDE REGEX "/stdshaders/" )
list( FILTER SRC_FILES 					EXCLUDE REGEX "/shaderlib/" )
list( FILTER PUBLIC_SRC_FILES 			EXCLUDE REGEX "/shaderlib/" )
list( FILTER VIEW_ONLY_FILES 			EXCLUDE REGEX "/shaderlib/" )
list( FILTER PUBLIC_VIEW_ONLY_FILES 	EXCLUDE REGEX "/shaderlib/" )

filter_platform_files( SRC_FILES        		${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_SRC_FILES 		${PLATFORM_SRC_DIR} )
filter_platform_files( GENERATED_FILES 			${PLATFORM_SRC_DIR} )
filter_platform_files( VIEW_ONLY_FILES 			${PLATFORM_SRC_DIR} )
filter_platform_files( PUBLIC_VIEW_ONLY_FILES 	${PLATFORM_SRC_DIR} )

# Just add the source files to the project as documents
set_source_files_properties( ${VIEW_ONLY_FILES}     		PROPERTIES      VS_TOOL_OVERRIDE "None" )
set_source_files_properties( ${PUBLIC_VIEW_ONLY_FILES}     	PROPERTIES      VS_TOOL_OVERRIDE "None" ) 


#
# Add commands for EcsCompiler
#
add_shadercompiler_commands( ${SRC_DIR} ${GENERATED_DIR} GENERATED_FILES ${SRC_FILES} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} ${VIEW_ONLY_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} ${PUBLIC_VIEW_ONLY_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )

set( FULL_SRC_FILES                                     					${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} ${VIEW_ONLY_FILES} ${PUBLIC_VIEW_ONLY_FILES} )
add_library( materialsystem SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( materialsystem PRIVATE 							pch_materialsystem.h )
set_target_properties( materialsystem PROPERTIES FOLDER         			Engine )
set_target_properties( materialsystem PROPERTIES PREFIX 					"" )

target_link_libraries( materialsystem PRIVATE								tier0 tier1 interfaces shaderlib shadercache smatdoc stexdoc ${CMAKE_DL_LIBS} )
add_common_target_defines( materialsystem )
add_dependencies( materialsystem											stdshaders studiorender )


#
# Install
#
inherit_install_files( materialsystem											TARGETS tier0 tier1 interfaces shaderlib shadercache smatdoc stexdoc stdshaders studiorender )

# Binaries
install_files( materialsystem TARGETS RUNTIME materialsystem 					DESTINATION bin/${PLATFORM_BIN_DIR}/ )
if ( INSTALL_PDB )
    install_files( materialsystem FILES $<TARGET_PDB_FILE:materialsystem>     	DESTINATION bin/${PLATFORM_BIN_DIR}/ )
endif()