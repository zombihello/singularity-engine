#include "pch_appframework.h"
#include "tier1/convar.h"
#include "tier1/filetools.h"
#include "cvar/icvar.h"
#include "filesystem/ifilesystem.h"
#include "appframework/appframework.h"

#if ENABLE_LOGGING
	#include "tier0/ilogger.h"
	#include "tier0/crashdump.h"
	#include "tier0/icommandline.h"

//-----------------------------------------------------------------------------
// A log output into a file
// NOTE: For use must be connected Tier1
//-----------------------------------------------------------------------------
class CLogOutputFile : public CBaseLogOutput<ILogOutput>
{
public:
	// ILogOutput interface
	virtual void Print( logLevel_t level, const char* pMessage ) override;

	CLogOutputFile( const char* pPath );

private:
	TRefPtr<IStreamDataWriter> pFile;
};

/*
==================
CLogOutputFile::CLogOutputFile
==================
*/
CLogOutputFile::CLogOutputFile( const char* pPath )
{
	Assert( g_pFileSystem );
	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "AppFramework: Failed to open log file '%s'", pPath );
	}
}

/*
==================
CLogOutputFile::Print
==================
*/
void CLogOutputFile::Print( logLevel_t level, const char* pMessage )
{
	if ( pFile )
	{
		pFile->Write( (void*)pMessage, S_Strlen( pMessage ) * sizeof( char ) );
	}
}
#endif	// ENABLE_LOGGING

/*
==================
CApplication::Create
==================
*/
bool CApplication::Create()
{
	// Load base application systems
	appSystemInfo_t appSystemInfos[] = {
		{ "cvar" DLL_EXT_STRING, CVAR_QUERY_INTERFACE_VERSION },  // This one must be first
		{ "filesystem" DLL_EXT_STRING, FILESYSTEM_INTERFACE_VERSION },
		{ "cvar" DLL_EXT_STRING, CVAR_INTERFACE_VERSION },
		{ "", "" }	// Required to terminate the list
	};

	// Add all systems from the array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}
	return true;
}

/*
==================
CApplication::PreInit
==================
*/
bool CApplication::PreInit()
{
	if ( !ConnectTier1( GetFactory() ) )
	{
		return false;
	}
	return true;
}

/*
==================
CApplication::PostInit
==================
*/
bool CApplication::PostInit()
{
	// Get the executable path and append at the end path separator if it need
	eastl::string exePath;
	S_GetFilePath( Sys_GetExecutablePath(), exePath, false );
	S_AppendPathSeparator( exePath );

	// Add search paths
	g_pFileSystem->AddSearchPath( exePath.c_str(), "enginebin" );
	g_pFileSystem->AddSearchPath( "core", "core" );

	// Setup a log file
#if ENABLE_LOGGING
	if ( pLogFileName && S_Strlen( pLogFileName ) > 0 && ( DEBUG || CommandLine()->HasParam( "log" ) ) )
	{
		eastl::string logFilePath = S_Sprintf( "//BASE_PATH/logs/%s.log", pLogFileName );
		pLogOutputFile			  = new CLogOutputFile( logFilePath.c_str() );
		CrashDump_AddLogFile( logFilePath.c_str() );
		Logger()->AddOutput( pLogOutputFile );
	}
#endif	// ENABLE_LOGGING
	return true;
}

/*
==================
CApplication::Main
==================
*/
int32 CApplication::Main()
{
	// Now all base application systems are loaded, we can now load up main libraries
	Assert( pChildAppSystemGroup );
	pChildAppSystemGroup->Setup( this );
	return pChildAppSystemGroup->Run();
}

/*
==================
CApplication::PostShutdown
==================
*/
void CApplication::PostShutdown()
{
#if ENABLE_LOGGING
	if ( pLogOutputFile )
	{
		Logger()->RemoveOutput( pLogOutputFile );
		delete pLogOutputFile;
		pLogOutputFile = NULL;
	}
#endif	// ENABLE_LOGGING

	DisconnectTier1();
}
