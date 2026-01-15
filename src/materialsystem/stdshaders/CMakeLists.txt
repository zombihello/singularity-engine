cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.makefile" "${SRC_DIR}/*.shader" "${SRC_DIR}/*.hlsl" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/materialsystem/*.h" "${PUBLIC_SRC_DIR}/materialsystem/*.inl" "${PUBLIC_SRC_DIR}/materialsystem/*.cpp" 
										"${PUBLIC_SRC_DIR}/materialsystem/*.hlsl" "${PUBLIC_SRC_DIR}/core/*.cpp" )
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
# Add commands for EcsCompiler
#
add_shadercompiler_commands( ${SRC_DIR} ${GENERATED_DIR} GENERATED_FILES ${SRC_FILES} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES                                     				${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( stdshaders SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( stdshaders PRIVATE 							pch_stdshaders.h )
set_target_properties( stdshaders PROPERTIES FOLDER         			Engine/Shaders )
set_target_properties( stdshaders PROPERTIES PREFIX 					"" )

target_link_libraries( stdshaders PRIVATE								core stdlib interfaces shaderlib shadercache ${CMAKE_DL_LIBS} )
add_common_target_defines( stdshaders )


#
# Install
#
inherit_install_files( stdshaders										TARGETS core stdlib interfaces shaderlib shadercache )

# Binaries
install_files( stdshaders TARGETS RUNTIME stdshaders 					DESTINATION	bin/${PLATFORM_BIN_DIR}/ )