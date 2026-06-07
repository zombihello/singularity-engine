cmake_minimum_required(VERSION 3.5)
project(wildmatch LANGUAGES C CXX)

set(${PROJECT_NAME}_MAJOR_VERSION 1)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_LEVEL 0)

set(CMAKE_SKIP_INSTALL_RPATH TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

enable_testing()

# BS yehor.pohuliaka - BEGIN - Wildmatch integration
if (NOT MSVC)
	add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()
# BS yehor.pohuliaka - END - Wildmatch integration
include_directories(${CMAKE_SOURCE_DIR})

install(FILES README.md DESTINATION share/doc/${PROJECT_NAME})

add_subdirectory(wildmatch)
# BS yehor.pohuliaka - Wildmatch integration
#add_subdirectory(tests)
add_subdirectory(cmake)
