#include "pch_cvar.h"
#include <sstream>

#include "stdlib/buildnum.h"
#include "core/version.h"
#include "filesystem/ifilesystem.h"

/*
==================
Version command
==================
*/
CON_COMMAND( version, "Print version info string", FCVAR_NONE )
{
	TBuildNumber<ENGINE_GOLDDATE> engineBuildNumber;
	Msg( "Singularity Engine " ENGINE_VERSION_STRING " build %i (" __DATE__ " " __TIME__ ")", engineBuildNumber.GetBuildNumber() );
}

/*
==================
Quit command
==================
*/
CON_COMMAND( quit, "Quit from the game", FCVAR_NONE )
{
	Sys_RequestExit( false );
}

/*
==================
Exec command
==================
*/
CON_COMMAND( exec, "Execute a command file", FCVAR_NONE )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	if ( argc < 1 || !argv )
	{
		Msg( "Cvar: exec <path> : Execute a command file" );
		return;
	}

	// Open a command file
	TRefPtr<IStreamDataReader> file = g_pFileSystem->CreateFileReader( argv[0] );
	if ( file )
	{
		// Read whole file into buffer
		std::string buffer;
		buffer.resize( file->GetSize() / sizeof( char ) );
		file->Read( buffer.data(), file->GetSize() );

		// Executing a file
		Msg( "Cvar: exec %s: Executing", argv[0] );
		std::stringstream sstream( buffer );
		std::string		  line;
		while ( std::getline( sstream, line ) )
		{
			// We throw away \r
			if ( !line.empty() && line.back() == '\r' )
			{
				line.pop_back();
			}

			// We ignore line if it starts with C++ comment (//)
			if ( line.rfind( "//", 0 ) == std::string::npos )
			{
				g_pCvar->Exec( line.c_str() );
			}
		}

		// We are done!
		return;
	}

	// We failed to open file, it is bad
	Warning( "Cvar: exec %s: Failed to open file", argv[0] );
}
