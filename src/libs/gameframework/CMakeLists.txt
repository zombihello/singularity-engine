cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( Flecs REQUIRED )

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
set( PUBLIC_SRC_PATTERNS				"${PUBLIC_SRC_DIR}/libs/gameframework/*.h" "${PUBLIC_SRC_DIR}/libs/gameframework/*.inl" 
										"${PUBLIC_SRC_DIR}/libs/gameframework/*.cpp" "${PUBLIC_SRC_DIR}/libs/gameframework/*.ecs" )
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
add_ecscompiler_commands( ${PUBLIC_SRC_DIR}/libs/gameframework ${GENERATED_DIR} GENERATED_FILES ${PUBLIC_SRC_FILES} )


#
# Source groups
#
source_group( TREE "${SRC_DIR}"         PREFIX "src"            FILES   ${SRC_FILES} )
source_group( TREE "${PUBLIC_SRC_DIR}"  PREFIX "public"         FILES   ${PUBLIC_SRC_FILES} )
source_group( TREE "${GENERATED_DIR}"   PREFIX "generated"      FILES   ${GENERATED_FILES} )


set( FULL_SRC_FILES												${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( gameframework STATIC 	                      		${FULL_SRC_FILES} )
target_precompile_headers( gameframework PRIVATE 				pch_gameframework.h )
set_target_properties( gameframework PROPERTIES FOLDER			Engine/Libraries )
set_target_properties( gameframework PROPERTIES PREFIX 			"" )

target_include_directories( gameframework PUBLIC				${THIRDPARTY_INCLUDES} )
target_link_libraries( gameframework PUBLIC 					stdlib ${THIRDPARTY_LIBS} )
add_common_target_defines( gameframework )


#
# Install
#
# Binaries
install_files( gameframework FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}		DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( gameframework FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}	DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( gameframework FILES ${FLECS_INSTALL_LICENSES}			DESTINATION 	licenses/flecs/ )