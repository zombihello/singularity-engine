#
# Filter platform specific files in /platforms/ directories
#
function( filter_platform_files FILES PLATFORM_SRC_DIR )
    if( NOT DEFINED ${FILES} )
        message( FATAL_ERROR "filter_platform_files: file list '${FILES}' is not defined" )
    endif()
    if( NOT PLATFORM_SRC_DIR )
        message( FATAL_ERROR "filter_platform_files: PLATFORM_SRC_DIR is required" )
    endif()

    set( PLATFORM_FILES "${${FILES}}" )
    list( FILTER PLATFORM_FILES INCLUDE REGEX "/platforms/${PLATFORM_SRC_DIR}/" )
    foreach( PLATFORM_GENERIC_DIR IN LISTS PLATFORM_GENERIC_DIRS )
        set( GENERIC_PLATFORM_FILES "${${FILES}}" )
        list( FILTER GENERIC_PLATFORM_FILES INCLUDE REGEX "/platforms/${PLATFORM_GENERIC_DIR}/" )
        list( APPEND PLATFORM_FILES ${GENERIC_PLATFORM_FILES} ) 
    endforeach()

    set( RESULT_FILES "${${FILES}}" )
    list( FILTER RESULT_FILES EXCLUDE REGEX "/platforms/" )
    list( APPEND RESULT_FILES ${PLATFORM_FILES} )
    set( ${FILES} "${RESULT_FILES}" PARENT_SCOPE )
endfunction()


#
# Get branch name from Git
#
function( git_get_branch RESULT )
    set( GIT_BRANCH "unknown" )
    if ( GIT_FOUND AND EXISTS "${ROOT_DIR}/.git" )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${ROOT_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    set( ${RESULT} ${GIT_BRANCH} PARENT_SCOPE )
endfunction()


#
# Get current commit from Git
#
function( git_get_commit RESULT )
    set( GIT_COMMIT "unknown" )
    if ( GIT_FOUND AND EXISTS "${ROOT_DIR}/.git" )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${ROOT_DIR}
            OUTPUT_VARIABLE GIT_COMMIT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    set( ${RESULT} ${GIT_COMMIT} PARENT_SCOPE )
endfunction()


#
# Add to a target common defines
#
function( add_common_target_defines TARGET_NAME )
    if( NOT TARGET ${TARGET_NAME} )
        message( FATAL_ERROR "add_common_target_defines: target '${TARGET_NAME}' does not exist" )
    endif()

    get_target_property( TARGET_TYPE ${TARGET_NAME} TYPE )
    string( TOLOWER "${TARGET_NAME}" TARGET_NAME_LOWER )
    string( TOUPPER "${TARGET_NAME}" TARGET_NAME_UPPER )

    target_compile_definitions( ${TARGET_NAME} PRIVATE      TARGET_NAME="${TARGET_NAME_LOWER}" )
    if ( TARGET_TYPE STREQUAL "STATIC_LIBRARY" )
        target_compile_definitions( ${TARGET_NAME} PRIVATE  BUILD_STATIC_LIB )
    elseif( TARGET_TYPE STREQUAL "SHARED_LIBRARY" )
        target_compile_definitions( ${TARGET_NAME} PRIVATE  ${TARGET_NAME_UPPER}_DLL_EXPORT )
    endif()
endfunction()


#
# Append unique target property
# Usage: append_unique_property( <TARGET> <PROPERTY> [DATA, ...] )
#
function( append_unique_property DEST_TARGET DEST_IS_ADDED PROPERTY )
    if ( NOT TARGET ${DEST_TARGET} )
        message( FATAL_ERROR "append_unique_property: target '${DEST_TARGET}' does not exist" )
    endif()
    set( ${DEST_IS_ADDED}                   OFF PARENT_SCOPE )

    get_property( TARGET_PROP_DATA  TARGET "${DEST_TARGET}" PROPERTY "${PROPERTY}")
    set( UNIQUE_LIST                        "" )
    foreach ( ARG IN LISTS ARGN )
        list( FIND TARGET_PROP_DATA         "${ARG}" IDX )
        if ( IDX EQUAL -1 )
            list( APPEND UNIQUE_LIST        "${ARG}" )
            list( APPEND TARGET_PROP_DATA   "${ARG}" )
        endif()
    endforeach()

    if ( UNIQUE_LIST )
        set( ${DEST_IS_ADDED}               ON PARENT_SCOPE )
    endif()

    foreach( DATA IN LISTS UNIQUE_LIST )
        set_property( TARGET "${DEST_TARGET}" APPEND PROPERTY "${PROPERTY}" "${DATA}" )
    endforeach()
endfunction()


#
# Add install files
# Usage: install_files( <TARGET> FILES <FILE_PATH, ...> [DESTINATION <DEST_PATH>] [CONFIGURATIONS <CONFIGURATION, ...>] )
#
function( install_files DEST_TARGET )
    if ( NOT TARGET ${DEST_TARGET} )
        message( FATAL_ERROR "install_files: target '${DEST_TARGET}' does not exist" )
    endif()
    if ( NOT ARGN )
        return()
    endif()

    # Parse arguments and grab from there files, destination and configurations
    set( ARGS_OPTIONS )
    set( ARGS_ONE_VALUE_KEYWORDS        DESTINATION DIRECTORY )
    set( ARGS_MULTI_VALUE_KEYWORDS      TARGETS FILES CONFIGURATIONS )
    cmake_parse_arguments( ARGS "${ARGS_OPTIONS}" "${ARGS_ONE_VALUE_KEYWORDS}" "${ARGS_MULTI_VALUE_KEYWORDS}" ${ARGN} )

    # Handle case when arguments can be unparsed
    if ( ARGS_UNPARSED_ARGUMENTS )
        message( FATAL_ERROR "install_files: unknown arguments: ${ARGS_UNPARSED_ARGUMENTS}"  )
    endif()

    if ( NOT ARGS_DESTINATION )
        set( ARGS_DESTINATION           "." )
    endif()
    if ( NOT ARGS_CONFIGURATIONS )
        set( ARGS_CONFIGURATIONS        "GENERIC" )
    endif()

    # Validate mutually exclusive TARGETS vs FILES
    set( NUM_ACTIVE_ARGS_KINDS              0 )
    if ( ARGS_TARGETS )
        math( EXPR NUM_ACTIVE_ARGS_KINDS    "${NUM_ACTIVE_ARGS_KINDS} + 1" )
    endif()
    if ( ARGS_FILES )
        math( EXPR NUM_ACTIVE_ARGS_KINDS    "${NUM_ACTIVE_ARGS_KINDS} + 1" )
    endif()
    if ( ARGS_DIRECTORY )
        math( EXPR NUM_ACTIVE_ARGS_KINDS    "${NUM_ACTIVE_ARGS_KINDS} + 1" )
    endif()

    if ( NUM_ACTIVE_ARGS_KINDS EQUAL 0 )
        return()
    elseif ( NOT NUM_ACTIVE_ARGS_KINDS EQUAL 1 )
        message( FATAL_ERROR "install_files: specify exactly one of TARGETS, FILES, or DIRECTORY" )
    endif()

    set( INSTALL_DATA_TYPE              "" )
    set( INSTALL_DATA_LIST              "" )
    if ( ARGS_TARGETS )     
        set( ARGS_TARGETS_OPTIONS )
        set( ARGS_TARGETS_ONE_VALUE_KEYWORDS        RUNTIME ARCHIVE )
        set( ARGS_TARGETS_MULTI_VALUE_KEYWORDS )
        cmake_parse_arguments( ARGS_TARGETS "${ARGS_TARGETS_OPTIONS}" "${ARGS_TARGETS_ONE_VALUE_KEYWORDS}" "${ARGS_TARGETS_MULTI_VALUE_KEYWORDS}" ${ARGS_TARGETS} )
        if( ARGS_TARGETS_UNPARSED_ARGUMENTS )
            list( LENGTH ARGS_TARGETS_UNPARSED_ARGUMENTS    ARGS_TARGETS_UNPARSED_ARGUMENTS_LENGTH )
            if ( ARGS_TARGETS_UNPARSED_ARGUMENTS_LENGTH GREATER 1 )
                message( FATAL_ERROR "install_files: unknown arguments: ${ARGS_TARGETS_UNPARSED_ARGUMENTS}" )
            endif()
        endif()

        if ( ARGS_TARGETS_RUNTIME AND ARGS_TARGETS_ARCHIVE )
            message( FATAL_ERROR "install_files: specify at least one: RUNTIME <path> or ARCHIVE <path>" )
        endif()

        if ( ARGS_TARGETS_RUNTIME )
            set( INSTALL_DATA_TYPE          "TARGETS_RUNTIME" )
            set( INSTALL_DATA_LIST          ${ARGS_TARGETS_RUNTIME} )
        elseif( ARGS_TARGETS_ARCHIVE )
            set( INSTALL_DATA_TYPE          "TARGETS_ARCHIVE" )
            set( INSTALL_DATA_LIST          ${ARGS_TARGETS_ARCHIVE} )
        else()
            set( INSTALL_DATA_TYPE          "TARGETS" )
            set( INSTALL_DATA_LIST          ${ARGS_TARGETS} )
        endif()
    elseif( ARGS_FILES )
        set( INSTALL_DATA_TYPE          "FILES" )
        set( INSTALL_DATA_LIST          ${ARGS_FILES} )
    elseif( ARGS_DIRECTORY )
        set( INSTALL_DATA_TYPE          "DIRECTORY" )
        set( INSTALL_DATA_LIST          ${ARGS_DIRECTORY} )
    else()
        message( FATAL_ERROR "install_files: unknown install data type" )
    endif()

    # Append install files/targets into the target properties
    foreach ( CURRENT_ARGS_CONFIGURATION IN LISTS ARGS_CONFIGURATIONS )
        if ( CURRENT_ARGS_CONFIGURATION STREQUAL "GENERIC" )
            set( CONFIGURATION_SUFFIX       "_" )
        else()
            set( CONFIGURATION_SUFFIX       "_${CURRENT_ARGS_CONFIGURATION}_" )
        endif()

        string( TOUPPER                     "INSTALL${CONFIGURATION_SUFFIX}${INSTALL_DATA_TYPE}" CURRENT_PROPERTY )
        foreach( DATA IN LISTS INSTALL_DATA_LIST )
            append_unique_property( "${DEST_TARGET}" DATA_IS_ADDED "${CURRENT_PROPERTY}" "${DATA}" )
            if ( DATA_IS_ADDED )
                set_property( TARGET "${DEST_TARGET}" APPEND PROPERTY "${CURRENT_PROPERTY}" "${ARGS_DESTINATION}" )
            endif()
        endforeach()
    endforeach()
endfunction()


#
# Inherit install files
# Usage: inherit_install_files( <DEST_TARGET> [TARGETS <SRC_TARGETS, ...>] [DESTINATION <DESTINATION>] )
#
function( inherit_install_files DEST_TARGET )
    if ( NOT TARGET ${DEST_TARGET} )
        message( FATAL_ERROR "inherit_install_files: target '${DEST_TARGET}' does not exist" )
    endif()

    set( DEST_CONFIGURATIONS                "GENERIC" )
    if ( CMAKE_CONFIGURATION_TYPES )
        list( APPEND DEST_CONFIGURATIONS    ${CMAKE_CONFIGURATION_TYPES} )
    elseif( CMAKE_BUILD_TYPE )
        list( APPEND DEST_CONFIGURATIONS    ${CMAKE_BUILD_TYPE} )
    else()
        message( FATAL_ERROR "inherit_install_files: unknown build configurations, CMAKE_CONFIGURATION_TYPES and CMAKE_BUILD_TYPE are invalid" )
    endif()

    # Parse arguments and grab from there targets and destination
    set( ARGS_OPTIONS )
    set( ARGS_ONE_VALUE_KEYWORDS        DESTINATION )
    set( ARGS_MULTI_VALUE_KEYWORDS      TARGETS )
    cmake_parse_arguments( ARGS "${ARGS_OPTIONS}" "${ARGS_ONE_VALUE_KEYWORDS}" "${ARGS_MULTI_VALUE_KEYWORDS}" ${ARGN} )

    # Handle case when arguments can be unparsed or wasn't set 'TARGETS'
    if ( ARGS_UNPARSED_ARGUMENTS )
        message( FATAL_ERROR "inherit_install_files: unknown arguments: ${ARGS_UNPARSED_ARGUMENTS}"  )
    endif()
    if ( NOT ARGS_TARGETS )
        message( FATAL_ERROR "inherit_install_files: specify TARGETS" )
    endif()

    set( INSTALL_DATA_TYPES                 "TARGETS" "TARGETS_RUNTIME" "TARGETS_ARCHIVE" "FILES" "DIRECTORY" )
    foreach ( SRC_TARGET IN LISTS ARGS_TARGETS )
        if ( NOT TARGET ${SRC_TARGET} )
            message( FATAL_ERROR "inherit_install_files: target '${SRC_TARGET}' does not exist" )
        endif()

        foreach ( CURRENT_CONFIGURATION IN LISTS DEST_CONFIGURATIONS )
            if ( CURRENT_CONFIGURATION STREQUAL "GENERIC" )
                set( CONFIGURATION_SUFFIX               "_" )
            else()
                set( CONFIGURATION_SUFFIX               "_${CURRENT_CONFIGURATION}_" )
            endif()

            foreach( INSTALL_DATA_TYPE IN LISTS INSTALL_DATA_TYPES )
                string( TOUPPER                         "INSTALL${CONFIGURATION_SUFFIX}${INSTALL_DATA_TYPE}" CURRENT_PROPERTY )
                get_property( SRC_TARGET_PROP_DATA TARGET "${SRC_TARGET}" PROPERTY "${CURRENT_PROPERTY}" )
                if ( SRC_TARGET_PROP_DATA )
                    list( LENGTH                        SRC_TARGET_PROP_DATA SRC_TARGET_PROP_DATA_LENGTH )
                    math( EXPR                          SRC_TARGET_PROP_DATA_MOD "${SRC_TARGET_PROP_DATA_LENGTH} % 2" )
                    math( EXPR                          SRC_TARGET_PROP_LAST_DATA "${SRC_TARGET_PROP_DATA_LENGTH} - 2" )
                    if ( NOT SRC_TARGET_PROP_DATA_MOD EQUAL 0 )
                        message( FATAL_ERROR "inherit_install_files: pairs list corrupted in '${CURRENT_PROPERTY}', odd number of elements (${SRC_TARGET_PROP_DATA_LENGTH})" )
                    endif()

                    foreach( INDEX RANGE 0 ${SRC_TARGET_PROP_LAST_DATA} 2 )
                        math( EXPR                      NEXT_INDEX "${INDEX} + 1" )
                        list( GET                       SRC_TARGET_PROP_DATA ${INDEX}       DATA )
                        list( GET                       SRC_TARGET_PROP_DATA ${NEXT_INDEX}  DEST_DIR )

                        if ( ARGS_DESTINATION )
                            set( DEST_DIR               "${ARGS_DESTINATION}/${DEST_DIR}" )
                        endif()

                        append_unique_property( "${DEST_TARGET}" DATA_IS_ADDED "${CURRENT_PROPERTY}" "${DATA}" )
                        if ( DATA_IS_ADDED )
                            set_property( TARGET "${DEST_TARGET}" APPEND PROPERTY "${CURRENT_PROPERTY}" "${DEST_DIR}" )
                        endif()
                    endforeach()
                endif()
            endforeach()
        endforeach()
    endforeach()
endfunction()


#
# Install a target
# Usage: install_target( <TARGET> [COMPONENT <INSTALL_COMPONENT_NAME>] [DESTINATION <DEST_SUB_DIRECTORY>] [CONFIGURATIONS <CONFIGURATION, ...>] )
#
function( install_target DEST_TARGET )
    if ( NOT TARGET ${DEST_TARGET} )
        message( FATAL_ERROR "install_target: target '${DEST_TARGET}' does not exist" )
    endif()
    
    # Parse arguments and grab from there some parameters
    set( ARGS_OPTIONS )
    set( ARGS_ONE_VALUE_KEYWORDS        COMPONENT DESTINATION )
    set( ARGS_MULTI_VALUE_KEYWORDS      CONFIGURATIONS )
    cmake_parse_arguments( ARGS "${ARGS_OPTIONS}" "${ARGS_ONE_VALUE_KEYWORDS}" "${ARGS_MULTI_VALUE_KEYWORDS}" ${ARGN} )

    # Handle case when arguments can be unparsed
    if ( ARGS_UNPARSED_ARGUMENTS )
        message( FATAL_ERROR "install_target: unknown arguments: ${ARGS_UNPARSED_ARGUMENTS}"  )
    endif()

    if ( NOT ARGS_CONFIGURATIONS )
        set( ARGS_CONFIGURATIONS                "GENERIC" )
        if ( CMAKE_CONFIGURATION_TYPES )
            list( APPEND ARGS_CONFIGURATIONS    ${CMAKE_CONFIGURATION_TYPES} )
        elseif( CMAKE_BUILD_TYPE )
            list( APPEND ARGS_CONFIGURATIONS    ${CMAKE_BUILD_TYPE} )
        else()
            message( FATAL_ERROR "install_target: unknown build configurations, CMAKE_CONFIGURATION_TYPES and CMAKE_BUILD_TYPE are invalid" )
        endif()
    endif()

    # Make base install parameters
    if ( ARGS_COMPONENT )
        set( INSTALL_COMPONENT_PARAMS      COMPONENT ${ARGS_COMPONENT} )
    else()
        set( INSTALL_COMPONENT_PARAMS      COMPONENT ${DEST_TARGET} )
    endif()

    # Setup install of the target
    set( INSTALL_DATA_TYPES                     "TARGETS" "TARGETS_RUNTIME" "TARGETS_ARCHIVE" "FILES" "DIRECTORY" )
    foreach ( CURRENT_CONFIGURATION IN LISTS ARGS_CONFIGURATIONS )
        if ( CURRENT_CONFIGURATION STREQUAL "GENERIC" )
            set( CONFIGURATION_SUFFIX           "_" )
            set( INSTALL_PARAMS                 ${INSTALL_COMPONENT_PARAMS} )
        else()
            set( CONFIGURATION_SUFFIX           "_${CURRENT_CONFIGURATION}_" )
            set( INSTALL_PARAMS                 ${INSTALL_COMPONENT_PARAMS} CONFIGURATIONS ${CURRENT_CONFIGURATION} )
        endif()

        foreach( INSTALL_DATA_TYPE IN LISTS INSTALL_DATA_TYPES )
            string( TOUPPER                     "INSTALL${CONFIGURATION_SUFFIX}${INSTALL_DATA_TYPE}" CURRENT_PROPERTY )
            get_property( INSTALL_PROP_DATA TARGET "${DEST_TARGET}" PROPERTY "${CURRENT_PROPERTY}" )
            if ( INSTALL_PROP_DATA )
                list( LENGTH                    INSTALL_PROP_DATA INSTALL_PROP_DATA_LENGTH )
                math( EXPR                      INSTALL_PROP_DATA_MOD "${INSTALL_PROP_DATA_LENGTH} % 2" )
                math( EXPR                      INSTALL_PROP_LAST_DATA "${INSTALL_PROP_DATA_LENGTH} - 2" )
                if ( NOT INSTALL_PROP_DATA_MOD EQUAL 0 )
                    message( FATAL_ERROR "install_target: pairs list corrupted in '${CURRENT_PROPERTY}', odd number of elements (${INSTALL_PROP_DATA_LENGTH})" )
                endif()

                set( INSTALL_DATA_PARAMS )
                if ( INSTALL_DATA_TYPE STREQUAL "TARGETS_RUNTIME" )
                    set( INSTALL_DATA_PARAMS      RUNTIME )
                    set( INSTALL_DATA_TYPE        TARGETS )
                elseif ( INSTALL_DATA_TYPE STREQUAL "TARGETS_ARCHIVE" )
                    set( INSTALL_DATA_PARAMS      ARCHIVE )
                    set( INSTALL_DATA_TYPE        TARGETS )
                endif()

                foreach( INDEX RANGE 0 ${INSTALL_PROP_LAST_DATA} 2 )
                    math( EXPR                    NEXT_INDEX "${INDEX} + 1" )
                    list( GET                     INSTALL_PROP_DATA ${INDEX}       DATA )
                    list( GET                     INSTALL_PROP_DATA ${NEXT_INDEX}  DEST_DIR )

                    if ( DEST_DIR STREQUAL "." )
                        list( APPEND INSTALL_PARAMS         DESTINATION "$<TARGET_FILE_DIR:${DEST_TARGET}>" )
                    elseif ( IS_ABSOLUTE ${DEST_DIR} )
                        list( APPEND INSTALL_PARAMS         DESTINATION "${DEST_DIR}" )
                    elseif ( DEST_DIR AND ARGS_DESTINATION )
                        list( APPEND INSTALL_PARAMS         DESTINATION "${ARGS_DESTINATION}/${DEST_DIR}" )
                    endif()

                    install( ${INSTALL_DATA_TYPE} ${DATA} ${INSTALL_DATA_PARAMS} ${INSTALL_PARAMS} )
                endforeach()
            endif()
        endforeach()
    endforeach()
endfunction()


#
# Add commands to compile *.flex files
# Usage: add_flex_commands( ${<DEST_DIR>} <DEST_OUTPUT_FILES> ${<FILES, ...>} )
#
function( add_flex_commands DEST_DIR DEST_OUTPUT_FILES )
    set( FLEX_DIR       "${DEVTOOLS_DIR}/flex/" )
    set( FLEX_BIN_DIR   "${FLEX_DIR}/" )
    set( FLEX_EXE       "" )
    if ( PLATFORM_WINDOWS )
        set( FLEX_EXE   "${FLEX_BIN_DIR}/win_flex.exe" )
    else()
        message( FATAL_ERROR "add_flex_commands: unknown platform" )
    endif()

    if ( NOT EXISTS "${FLEX_EXE}" )
        message( WARNING "add_flex_commands: '${FLEX_EXE}' is missing" )
    endif()

    set( OUTPUT_FILES               ${${DEST_OUTPUT_FILES}} )
    foreach ( FILE IN LISTS ARGN )
        if ( FILE MATCHES "\\.flex$" )
            get_filename_component( FILE_NAME "${FILE}" NAME_WE )
            set( OUTPUT_FILE_CPP "${DEST_DIR}/${FILE_NAME}_flex.cpp" )
            add_custom_command( OUTPUT "${OUTPUT_FILE_CPP}"
                                COMMAND "${FLEX_EXE}" -o "${OUTPUT_FILE_CPP}" "${FILE}"
                                DEPENDS "${FILE}"
                                WORKING_DIRECTORY "${FLEX_BIN_DIR}" )

            set_source_files_properties( "${OUTPUT_FILE_CPP}" PROPERTIES GENERATED TRUE )
            list( APPEND OUTPUT_FILES "${OUTPUT_FILE_CPP}" )
        endif()
    endforeach()

    set( ${DEST_OUTPUT_FILES} ${OUTPUT_FILES} PARENT_SCOPE )
endfunction()


#
# Add commands to compile *.bison files
# Usage: add_bison_commands( ${<DEST_DIR>} <DEST_OUTPUT_FILES> ${<FILES, ...>} )
#
function( add_bison_commands DEST_DIR DEST_OUTPUT_FILES )
    set( BISON_DIR       "${DEVTOOLS_DIR}/bison/" )
    set( BISON_BIN_DIR   "${BISON_DIR}/bin/" )
    set( BISON_EXE       "" )
    if ( PLATFORM_WINDOWS )
        set( BISON_EXE   "${BISON_BIN_DIR}/bison.exe" )
    else()
        message( FATAL_ERROR "add_bison_commands: unknown platform" )
    endif()

    if ( NOT EXISTS "${BISON_EXE}" )
        message( WARNING "add_bison_commands: '${BISON_EXE}' is missing" )
    endif()

    set( OUTPUT_FILES               ${${DEST_OUTPUT_FILES}} )
    foreach ( FILE IN LISTS ARGN )
        if ( FILE MATCHES "\\.bison$" )
            get_filename_component( FILE_NAME "${FILE}" NAME_WE )
            set( OUTPUT_FILE_HEADER "${DEST_DIR}/${FILE_NAME}_bison.cpp.h" )
            set( OUTPUT_FILE_CPP    "${DEST_DIR}/${FILE_NAME}_bison.cpp" )
            add_custom_command( OUTPUT "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}"
                                COMMAND "${BISON_EXE}" --defines="${OUTPUT_FILE_HEADER}" -o "${OUTPUT_FILE_CPP}" "${FILE}"
                                DEPENDS "${FILE}"
                                WORKING_DIRECTORY "${BISON_BIN_DIR}" )

            set_source_files_properties( "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}" PROPERTIES GENERATED TRUE )
            list( APPEND OUTPUT_FILES "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}" )
        endif()
    endforeach()

    set( ${DEST_OUTPUT_FILES} ${OUTPUT_FILES} PARENT_SCOPE )
endfunction()

#
# Add commands to compile *.bison files
# Usage: add_ecscompiler_commands( ${<BASE_DIR>} ${<DEST_DIR>} <DEST_OUTPUT_FILES> ${<FILES, ...>} )
#
function( add_ecscompiler_commands BASE_DIR DEST_DIR DEST_OUTPUT_FILES )
    set( ECSCOMPILER_DIR            "${DEVTOOLS_DIR}/ecscompiler/" )
    set( ECSCOMPILER_BIN_DIR        "" )
    set( ECSCOMPILER_EXE            "" )
    if ( PLATFORM_WINDOWS )
        set( ECSCOMPILER_BIN_DIR    "${ECSCOMPILER_DIR}/bin/${PLATFORM_NAME}_release/" )
        set( ECSCOMPILER_EXE        "${ECSCOMPILER_BIN_DIR}/ecscompiler.exe" )
    else()
        message( FATAL_ERROR "add_ecscompiler_commands: unknown platform" )
    endif()

    if ( NOT EXISTS "${ECSCOMPILER_EXE}" )
        message( WARNING "add_ecscompiler_commands: '${ECSCOMPILER_EXE}' is missing, before compiling you shuild to build ecscompiler in release configuration" )
    endif()

    set( OUTPUT_FILES               ${${DEST_OUTPUT_FILES}} )
    foreach ( FILE IN LISTS ARGN )
        if ( FILE MATCHES "\\.ecs$" )
            get_filename_component( FILE_DIR "${FILE}" DIRECTORY )
            file( RELATIVE_PATH     SUB_DIR "${BASE_DIR}" "${FILE_DIR}"  )
            set( FILE_DEST_DIR      "${DEST_DIR}" )
            if ( SUB_DIR )
                set( FILE_DEST_DIR  "${FILE_DEST_DIR}/${SUB_DIR}" )
            endif()

            get_filename_component( FILE_NAME "${FILE}" NAME_WE )
            set( OUTPUT_FILE_HEADER "${FILE_DEST_DIR}/${FILE_NAME}.gen.h" )
            set( OUTPUT_FILE_CPP    "${FILE_DEST_DIR}/${FILE_NAME}.gen.cpp" )
            add_custom_command( OUTPUT "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}"
                                COMMAND "${ECSCOMPILER_EXE}" -file "${FILE}" -output "${FILE_DEST_DIR}"
                                DEPENDS "${FILE}"
                                WORKING_DIRECTORY "${ROOT_DIR}" )

            set_source_files_properties( "${OUTPUT_FILE_CPP}"                           PROPERTIES HEADER_FILE_ONLY TRUE )
            set_source_files_properties( "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}"   PROPERTIES GENERATED TRUE )
            list( APPEND OUTPUT_FILES "${OUTPUT_FILE_HEADER}" "${OUTPUT_FILE_CPP}" )
        endif()
    endforeach()

    set( ${DEST_OUTPUT_FILES} ${OUTPUT_FILES} PARENT_SCOPE )
endfunction()

#
# Add commands to compile *.bison files
# Usage: add_shadercompiler_commands( ${<BASE_DIR>} ${<DEST_DIR>} <DEST_OUTPUT_FILES> ${<FILES, ...>} )
#
function( add_shadercompiler_commands BASE_DIR DEST_DIR DEST_OUTPUT_FILES )
    set( SHADERCOMPILER_DIR            "${DEVTOOLS_DIR}/shadercompiler/" )
    set( SHADERCOMPILER_BIN_DIR        "" )
    set( SHADERCOMPILER_EXE            "" )
    if ( PLATFORM_WINDOWS )
        set( SHADERCOMPILER_BIN_DIR    "${SHADERCOMPILER_DIR}/bin/${PLATFORM_NAME}_release/" )
        set( SHADERCOMPILER_EXE        "${SHADERCOMPILER_BIN_DIR}/shadercompiler.exe" )
    else()
        message( FATAL_ERROR "add_shadercompiler_commands: unknown platform" )
    endif()

    if ( NOT EXISTS "${SHADERCOMPILER_EXE}" )
        message( WARNING "add_shadercompiler_commands: '${SHADERCOMPILER_EXE}' is missing, before compiling you shuild to build shadercompiler in release configuration" )
    endif()

    set( OUTPUT_FILES               ${${DEST_OUTPUT_FILES}} )
    foreach ( FILE IN LISTS ARGN )
        if ( FILE MATCHES "\\.shader$" )
            get_filename_component( FILE_DIR "${FILE}" DIRECTORY )
            file( RELATIVE_PATH     SUB_DIR "${BASE_DIR}" "${FILE_DIR}"  )
            set( FILE_DEST_DIR      "${DEST_DIR}" )
            if ( SUB_DIR )
                set( FILE_DEST_DIR  "${FILE_DEST_DIR}/${SUB_DIR}" )
            endif()

            get_filename_component( FILE_NAME "${FILE}" NAME_WE )
            set( OUTPUT_FILE_HEADER "${FILE_DEST_DIR}/${FILE_NAME}.gen.h" )
            add_custom_command( OUTPUT "${OUTPUT_FILE_HEADER}"
                                COMMAND "${SHADERCOMPILER_EXE}" -mode gencpp -file "${FILE}" -output "${FILE_DEST_DIR}"
                                DEPENDS "${FILE}"
                                WORKING_DIRECTORY "${ROOT_DIR}" )

            set_source_files_properties( "${OUTPUT_FILE_HEADER}" PROPERTIES GENERATED TRUE )
            list( APPEND OUTPUT_FILES "${OUTPUT_FILE_HEADER}" )
        endif()
    endforeach()

    set( ${DEST_OUTPUT_FILES} ${OUTPUT_FILES} PARENT_SCOPE )
endfunction()