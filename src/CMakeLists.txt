cmake_minimum_required( VERSION 3.19 )
set( PROJECT_NAME "singularity_engine" CACHE STRING "Name of the solution project" )
project( ${PROJECT_NAME} )

#
# Find third parties
#
find_package( Git )


#
# CMake settings
#
cmake_policy( SET CMP0177 NEW )
set( CMAKE_INCLUDE_CURRENT_DIR              ON )
set( CMAKE_CXX_STANDARD                     17 )
set( CMAKE_CXX_STANDARD_REQUIRED            ON )
set_property( GLOBAL PROPERTY USE_FOLDERS   ON )

# Set CMAKE_CONFIGURATION_TYPES only for multi-config generator
if ( CMAKE_CONFIGURATION_TYPES )
    set( CMAKE_CONFIGURATION_TYPES          "Debug;Release;Retail" CACHE STRING "Available build configurations" FORCE )
endif()

# Set default CMAKE_INSTALL_PREFIX if it wasn't set
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX               "${CMAKE_SOURCE_DIR}/" CACHE PATH "Default install path" FORCE )
endif()


#
# Disable PCH for source files in public folder
#
file( GLOB_RECURSE PUBLIC_FILES                         "public/*.h" "public/*.inl" "public/*.cpp" "public/*.c" )
set_source_files_properties( PUBLIC_FILES PROPERTIES    SKIP_PRECOMPILE_HEADERS ON )


#
# Options
#
option( INSTALL_SDK_TOOLS                               "Install sdk tools" ON )
option( INSTALL_DEV_TOOLS                               "Install dev tools" ON )
option( INSTALL_THIRDPARTY_TOOLS                        "Install third-party tools" ON )


#
# Paths
#
set( THIRDPARTY_DIR         ${CMAKE_SOURCE_DIR}/thirdparty/ )
set( ROOT_DIR               ${CMAKE_SOURCE_DIR}/../ )
set( PUBLIC_SRC_DIR         ${CMAKE_SOURCE_DIR}/public/ )
set( GAME_DIR               ${ROOT_DIR}/game/ )
set( DEVTOOLS_DIR           ${CMAKE_SOURCE_DIR}/devtools/ )
set( GENERATED_ROOT_DIR     ${CMAKE_BINARY_DIR} )

include_directories( ${CMAKE_SOURCE_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/public )
include_directories( ${CMAKE_BINARY_DIR} )


#
# Platform settings
#
set( PLATFORM_NAME                      "" )
set( PLATFORM_BIN_DIR                   "" )
set( PLATFORM_SRC_DIR                   "" )
set( PLATFORM_GENERIC_DIRS              "generic" "sdl" )
set( PLATFORM_WINDOWS   				OFF )
set( PLATFORM_32BIT     				0 )
set( PLATFORM_64BIT     				0 )

if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( PLATFORM_64BIT 1 )
elseif ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    set( PLATFORM_32BIT 1 )
else()
    message( SEND_ERROR "Unknown bit platform" )
endif()

# Windows
if ( CMAKE_SYSTEM_NAME MATCHES "Windows" )
    set( PLATFORM_WINDOWS   ON )
    set( PLATFORM_SRC_DIR   "windows" )

    # Visual Studio compiler
    if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
        add_compile_options( /MP /FC /fp:fast /external:W0 /external:anglebrackets )
        add_compile_options( $<$<CONFIG:Debug>:/Zi> $<$<CONFIG:Release>:/Zi> )
        add_compile_options( $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2> $<$<CONFIG:Retail>:/Ox> )
        add_compile_options( $<$<CONFIG:Debug>:/Ob0> $<$<OR:$<CONFIG:Release>,$<CONFIG:Retail>>:/Ob2> )
        add_link_options( $<$<CONFIG:Debug>:/DEBUG> $<$<CONFIG:Release>:/DEBUG> )

        add_compile_definitions( _CRT_SECURE_NO_WARNINGS )
        add_compile_definitions( _CRT_NONSTDC_NO_DEPRECATE )
        add_compile_definitions(
            "$<$<CONFIG:Debug>:DEBUG=1;RELEASE=0;RETAIL=0>"
            "$<$<CONFIG:Release>:DEBUG=0;RELEASE=1;RETAIL=0>"
            "$<$<CONFIG:Retail>:DEBUG=0;RELEASE=0;RETAIL=1>" )
    else()
        message( SEND_ERROR "Unknown compiler" )
    endif()

    if ( PLATFORM_64BIT )
        set( PLATFORM_NAME "win64" )   
    elseif ( PLATFORM_32BIT ) 
        set( PLATFORM_NAME "win32" )
    else()
        message( SEND_ERROR "Unknown bit platform" )
    endif()
else()
    message( SEND_ERROR "Unknown platform" )
endif()

set( PLATFORM_BIN_DIR "${PLATFORM_NAME}_$<LOWER_CASE:$<CONFIG>>" )
add_compile_definitions( PLATFORM_64BIT=${PLATFORM_64BIT} )
add_compile_definitions( PLATFORM_32BIT=${PLATFORM_32BIT} )
add_compile_definitions( PLATFORM_NAME="${PLATFORM_NAME}" )
add_compile_definitions( PLATFORM_BIN_DIR="${PLATFORM_BIN_DIR}" )
add_compile_definitions( PLATFORM_DIR="$<LOWER_CASE:${PLATFORM_NAME}>" )


#
# Configurations settings
#
set( DEBUG_NAME                             Debug )
set( RELEASE_NAME                           Release )
set( RETAIL_NAME                            Retail )
set( CMAKE_CXX_FLAGS_RETAIL                 "${CMAKE_CXX_FLAGS_RELEASE}" )
set( CMAKE_RC_FLAGS_RETAIL                  "${CMAKE_RC_FLAGS_RELEASE}" )
set( CMAKE_EXE_LINKER_FLAGS_RETAIL          "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" )
set( CMAKE_MODULE_LINKER_FLAGS_RETAIL       "${CMAKE_MODULE_LINKER_FLAGS_RELEASE}" )
set( CMAKE_SHARED_LINKER_FLAGS_RETAIL       "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" )
set( CMAKE_STATIC_LINKER_FLAGS_RETAIL       "${CMAKE_STATIC_LINKER_FLAGS_RELEASE}" )


#
# Include some cmake files
#
include( "${CMAKE_SOURCE_DIR}/cmake/utils.cmake" )
include( "${CMAKE_SOURCE_DIR}/cmake/thirdparty.cmake" )

# Set defines for git branch and commit
git_get_branch( GIT_BRANCH )
git_get_commit( GIT_COMMIT )
add_compile_definitions( GIT_BRANCH="${GIT_BRANCH}" )
add_compile_definitions( GIT_COMMIT="${GIT_COMMIT}" )


#
# Add project
#
# Engine
add_subdirectory( utils/interfaces )
add_subdirectory( tier1 )
add_subdirectory( tier0 )
add_subdirectory( filesystem )
add_subdirectory( cvar )
add_subdirectory( inputsystem )
add_subdirectory( utils/pixelformatinfos )
add_subdirectory( utils/shadercache )
add_subdirectory( studiorender/studioapi_vk )
add_subdirectory( utils/smdldoc )
add_subdirectory( studiorender )
add_subdirectory( materialsystem/shaderlib )
add_subdirectory( materialsystem/stdshaders )
add_subdirectory( utils/stexdoc )
add_subdirectory( utils/smatdoc )
add_subdirectory( materialsystem )
add_subdirectory( resourcesystem )
add_subdirectory( appframework )
add_subdirectory( utils/gameinfo )
add_subdirectory( launcher )

# Game
add_subdirectory( utils/sentdoc )
add_subdirectory( utils/smapdoc )
add_subdirectory( game/shared )
add_subdirectory( game/citadel )
add_subdirectory( launcher_main/citadel )

# Tools
add_subdirectory( utils/toolsinfo )
add_subdirectory( utils/parserlib )
add_subdirectory( tools/ecscompiler )
add_subdirectory( tools/resourcecompiler/compilers/sent )
add_subdirectory( tools/resourcecompiler/compilers/smap )
add_subdirectory( tools/resourcecompiler/compilers/smat )
add_subdirectory( tools/resourcecompiler/compilers/smdl )
add_subdirectory( tools/resourcecompiler/compilers/stex )
add_subdirectory( tools/resourcecompiler )
add_subdirectory( tools/shadercompiler/vk )
add_subdirectory( tools/shadercompiler )