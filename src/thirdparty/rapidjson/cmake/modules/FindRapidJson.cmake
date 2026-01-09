# ---------------------------------
#	[in] 	RAPIDJSON_PATH					- Root directory rapidjson
#	[out] 	RAPIDJSON_INCLUDE				- Directory with includes
# 	[out] 	RAPIDJSON_INSTALL_LICENSES		- Rapidjson licenses to install
#	[out]	RAPIDJSON_FOUND					- Is found rapidjson
# ---------------------------------

set( RAPIDJSON_FOUND 			OFF )
set( RAPIDJSON_SEARCH_PATHS 	${RAPIDJSON_PATH} )


#
# Find pathes
#
# Includes
find_path( 		RAPIDJSON_INCLUDE
				NO_DEFAULT_PATH
				NAMES "rapidjson/rapidjson.h"
				PATH_SUFFIXES src/include
				PATHS ${RAPIDJSON_SEARCH_PATHS} )

# Licenses
find_file( 		RAPIDJSON_LICENSE_TXT
				NO_DEFAULT_PATH
				NAMES "license.txt"
				PATHS ${RAPIDJSON_SEARCH_PATHS} )


if ( NOT RAPIDJSON_INCLUDE )
	message( SEND_ERROR "RapidJson :: Not found" )
	return()
else()
	set( RAPIDJSON_FOUND 			ON )
	set( RAPIDJSON_INSTALL_LICENSES	${RAPIDJSON_LICENSE_TXT} )
endif()
