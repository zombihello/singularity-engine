cmake_minimum_required( VERSION 3.19 )

#
# Find third parties
#
find_package( Compressonator REQUIRED )

set( THIRDPARTY_INCLUDES					${COMPRESSONATOR_INCLUDE} )
set( THIRDPARTY_LIBS						${COMPRESSONATOR_LIBS} )
set( THIRDPARTY_INSTALL_DEBUG_DLLS			${COMPRESSONATOR_INSTALL_DLLS_DEBUG} )
set( THIRDPARTY_INSTALL_RELEASE_DLLS		${COMPRESSONATOR_INSTALL_DLLS_RELEASE} )

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


set( FULL_SRC_FILES                                     							${SRC_FILES} ${PUBLIC_SRC_FILES} ${GENERATED_FILES} )
add_library( resourcecompiler_stex SHARED 	                      	    			${FULL_SRC_FILES} )
set_target_properties( resourcecompiler_stex PROPERTIES FOLDER         				"Tools/Resource Compiler" )
set_target_properties( resourcecompiler_stex PROPERTIES PREFIX 						"" )

target_include_directories( resourcecompiler_stex PRIVATE             				${THIRDPARTY_INCLUDES} )
target_link_libraries( resourcecompiler_stex PRIVATE								tier0 tier1 interfaces stexdoc ${THIRDPARTY_LIBS} ${CMAKE_DL_LIBS} )
add_common_target_defines( resourcecompiler_stex )


#
# Install
#
inherit_install_files( resourcecompiler_stex											TARGETS tier0 tier1 interfaces stexdoc )

# Binaries
install_files( resourcecompiler_stex TARGETS RUNTIME resourcecompiler_stex 				DESTINATION     bin/${PLATFORM_BIN_DIR}/ )
install_files( resourcecompiler_stex FILES ${THIRDPARTY_INSTALL_DEBUG_DLLS}				DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Debug )
install_files( resourcecompiler_stex FILES ${THIRDPARTY_INSTALL_RELEASE_DLLS}			DESTINATION 	bin/${PLATFORM_BIN_DIR}/	CONFIGURATIONS 	Release Retail )

# Linceses
install_files( resourcecompiler_stex DIRECTORY ${COMPRESSONATOR_INSTALL_LICENSES_DIR}	DESTINATION 	licenses/compressonator/ )