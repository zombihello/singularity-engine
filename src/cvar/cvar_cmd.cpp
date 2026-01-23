#include "pch_cvar.h"
#include <sstream>

#include "tier1/buildnum.h"
#include "tier0/version.h"
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
		eastl::string buffer;
		buffer.resize( file->GetSize() / sizeof( char ) );
		file->Read( buffer.data(), file->GetSize() );

		// Executing a file
		Msg( "Cvar: exec %s: Executing", argv[0] );
		if ( !buffer.empty() )
		{
			eastl::string commandBuffer;
			const char*	  pCfgStartLine = buffer.c_str();

			commandBuffer.resize( 1024 );
			while ( *pCfgStartLine != '\0' )
			{
				// Get current line
				const char* pCfgEndLine = pCfgStartLine;
				while ( *pCfgEndLine != '\0' && *pCfgEndLine != '\n' )
				{
					++pCfgEndLine;
				}

				uint64 cfgLenghtLine = (uint64)( pCfgEndLine - pCfgStartLine );
				if ( cfgLenghtLine > 0 && pCfgEndLine[-1] == '\r' )
				{
					--cfgLenghtLine;
				}

				// Remove spaces from beginning of the line
				while ( cfgLenghtLine > 0 && S_IsSpace( *pCfgStartLine ) )
				{
					++pCfgStartLine;
					--cfgLenghtLine;
				}

				// Skip empty line or comment
				if ( cfgLenghtLine == 0 || ( cfgLenghtLine >= 2 && pCfgStartLine[0] == '/' && pCfgStartLine[1] == '/' ) )
				{
					// Do nothing
				}
				// Otherwise execute the command
				else
				{
					if ( commandBuffer.size() < cfgLenghtLine + 1 )
					{
						commandBuffer.resize( cfgLenghtLine + 1 );
					}

					S_Strncpy( commandBuffer.data(), pCfgStartLine, cfgLenghtLine );
					commandBuffer[cfgLenghtLine] = '\0';
					g_pCvar->Exec( commandBuffer.c_str() );
				}

				// Go to the next line
				if ( *pCfgEndLine == '\n' )
				{
					++pCfgEndLine;
				}
				pCfgStartLine = pCfgEndLine;
			}
		}

		// We are done!
		return;
	}

	// We failed to open file, it is bad
	Warning( "Cvar: exec %s: Failed to open file", argv[0] );
}
