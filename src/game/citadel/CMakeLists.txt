cmake_minimum_required( VERSION 3.19 )

#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.ecs" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/tier0/*.cpp" )
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


#
# Add commands for EcsCompiler
#
add_ecscompiler_commands( ${SRC_DIR} ${GENERATED_DIR} GENERATED_FILES ${SRC_FILES} )


set( FULL_SRC_FILES                                     				${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( citadel SHARED 	                      	    			${FULL_SRC_FILES} )
target_precompile_headers( citadel PRIVATE 								pch_citadel.h )
set_target_properties( citadel PROPERTIES FOLDER         				Game )
set_target_properties( citadel PROPERTIES PREFIX 						"" )
set_target_properties( citadel PROPERTIES OUTPUT_NAME					game )

target_link_libraries( citadel PRIVATE									tier0 tier1 interfaces game_shared sentdoc smapdoc ${CMAKE_DL_LIBS} )
add_common_target_defines( citadel )


#
# Install
#
inherit_install_files( citadel											TARGETS tier0 tier1 interfaces sentdoc smapdoc )
inherit_install_files( citadel											TARGETS game_shared								DESTINATION citadel )

# Binaries
install_files( citadel TARGETS RUNTIME citadel 							DESTINATION	citadel/bin/${PLATFORM_BIN_DIR}/ )