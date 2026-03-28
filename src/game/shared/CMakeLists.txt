cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( Flecs REQUIRED )

set( THIRDPARTY_DEFINES					${FLECS_DEFINES} )
set( THIRDPARTY_INCLUDES		        ${FLECS_INCLUDE} )
set( THIRDPARTY_LIBS                    ${FLECS_LIBS} )
set( THIRDPARTY_INSTALL_DEBUG_DLLS      ${FLECS_INSTALL_DLLS_DEBUG} )
set( THIRDPARTY_INSTALL_RELEASE_DLLS    ${FLECS_INSTALL_DLLS_RELEASE} )


#
# Project settings
#
set( SRC_DIR							${CMAKE_CURRENT_SOURCE_DIR} )
set( GENERATED_DIR						${CMAKE_CURRENT_BINARY_DIR} )

set( SRC_PATTERNS						"${SRC_DIR}/*.h" "${SRC_DIR}/*.inl" "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.ecs" )
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/game/*.h" "${PUBLIC_SRC_DIR}/game/*.inl"
										"${PUBLIC_SRC_DIR}/game/*.cpp" "${PUBLIC_SRC_DIR}/game/*.ecs" )
set( GENERATED_SRC_PATTERNS				"${GENERATED_DIR}/*.h" "${GENERATED_DIR}/*.inl" "${GENERATED_DIR}/*.cpp"
										"${GENERATED_DIR}/*.hxx" "${GENERATED_DIR}/*.cxx" )


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
add_ecscompiler_commands( ${SRC_DIR} ${GENERATED_DIR} GENERATED_FILES ${SRC_FILES} )
add_ecscompiler_commands( ${PUBLIC_SRC_DIR}/game/shared ${GENERATED_DIR} GENERATED_FILES ${PUBLIC_SRC_FILES} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES												${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( game_shared STATIC 	                      		${FULL_SRC_FILES} )
target_precompile_headers( game_shared PRIVATE 					pch_game_shared.h )
set_target_properties( game_shared PROPERTIES FOLDER			Game )
set_target_properties( game_shared PROPERTIES PREFIX 			"" )

target_compile_definitions( game_shared PUBLIC					${THIRDPARTY_DEFINES} )
target_include_directories( game_shared PUBLIC					${THIRDPARTY_INCLUDES} )
target_link_libraries( game_shared PUBLIC 						tier1 ${THIRDPARTY_LIBS} )
add_common_target_defines( game_shared )


#
# Install
#
inherit_install_files( game_shared										TARGETS tier1 )

# Binaries
install_files( game_shared FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}		DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( game_shared FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}		DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( game_shared FILES ${FLECS_INSTALL_LICENSES}				DESTINATION 	licenses/flecs/ )