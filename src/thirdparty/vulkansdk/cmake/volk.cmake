if ( NOT TARGET Volk )
    set( SRC_FILES              ${VULKANSDK_VOLK_C_FILE} ${VULKANSDK_VOLK_H_FILE} )

    set( FULL_SRC_FILES         ${SRC_FILES} )
    source_group( "src"         FILES ${SRC_FILES} )

    add_library( Volk STATIC                        ${FULL_SRC_FILES} )
    set_target_properties( Volk PROPERTIES FOLDER   ThirdParty )
    target_include_directories( Volk PRIVATE        ${VULKANSDK_INCLUDE} )
    target_compile_definitions( Volk PRIVATE        ${VULKANSDK_VOLK_DEFINES} )
endif()