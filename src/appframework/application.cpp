#include "pch_appframework.h"
#include "tier0/icommandline.h"
#include "tier0/iprofiler.h"
#include "tier1/filetools.h"
#include "tier1/convar.h"
#include "appframework/application.h"
#if ENABLE_LOGGING
	#include "tier0/consoleio.h"
#endif	// ENABLE_LOGGING

//-----------------------------------------------------------------------------
// Gets at a factory that works just like FindSystem
//-----------------------------------------------------------------------------
// This function is used to make this system appear to the outside world to
// function exactly like the currently existing factory system
static CApplication* s_pCurrentApp = NULL;
void*				 AppCreateInterfaceFn( const char* pName )
{
	return s_pCurrentApp->FindSystem( pName );
}

/*
==================
CApplication::Init
==================
*/
void CApplication::Init()
{
	// The factory now uses the application
	s_pCurrentApp = this;

#if ENABLE_LOGGING
	// Add a log output into stdout if the application is console type
	if ( IsConsoleApp() )
	{
		Logger()->AddOutput( &GetLogOutputStdOut() );
	}

	// Enable developer messages if we in the debug configuration
	#if DEBUG
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, true );
	#endif	// DEBUG
#endif		// ENABLE_LOGGING

	// Initialize Tier0 and the command line
	InitTier0();
	CommandLine()->Init( pCommandLine );

	// Setup application information for the crash dump
	CrashDumpHandler()->SetAppInfo( GetCrashDumpInfo() );

	// Attach a console for I/O if it need (only for window apps)
#if ENABLE_LOGGING
	if ( IsWindowApp() && CommandLine()->HasParam( "stdout" ) )
	{
		Sys_SetupConsoleIO();
		Logger()->AddOutput( &GetLogOutputStdOut() );
	}
#endif	// ENABLE_LOGGING

// Disable ensures if it need
#if ENABLE_ENSURE
	if ( CommandLine()->HasParam( "noensure" ) )
	{
		Sys_SetEnsureAllow( false );
	}
#endif	// ENABLE_ENSURE

	// Initialize tier1
	AddGroup( &tier1SystemGroup );
	tier1SystemGroup.InitSystems();
	if ( !ConnectTier1( GetFactory() ) )
	{
		Sys_Error( "Failed to connect Tier1" );
	}

	// Register cvars
	const appInfo_t& appInfo = GetAppInfo();
	ConVar_Register( appInfo.baseConVarFlags, appInfo.pConVarsOverrider, appInfo.pCvarAccessor );

	// Setup a log file
#if ENABLE_LOGGING
	if ( DEBUG || CommandLine()->HasParam( "log" ) )
	{
		CLogOutputFile&			   logOutputFile = GetLogOutputFile();
		TRefPtr<IStreamDataWriter> pLogFile		 = logOutputFile.BeginLoggingToFile( S_Sprintf( "//base_path/logs/%s.log", appInfo.pAppName ).c_str() );
		if ( pLogFile )
		{
			CrashDumpHandler()->AddLogFile( pLogFile->GetPath() );
			Logger()->AddOutput( &logOutputFile );
		}
	}
#endif	// ENABLE_LOGGING

	// Print some information about the system
	systemMemoryInfo_t systemMemoryInfo = Mem_SystemMemoryInfo();
	Msg( "AppFramework: User: %s//%s", Sys_GetComputerName(), Sys_GetUserName() );
	Msg( "AppFramework: Total memory: %.2f MB", (double)systemMemoryInfo.totalSize / ( 1024 * 1024 ) );
	Msg( "AppFramework: Free memory: %.2f MB", (double)systemMemoryInfo.freeSize / ( 1024 * 1024 ) );

	// Set true in cheats and developer cvars if we in debug configuration
#if DEBUG
	CConVarRef cheatsRef( "cheats" );
	CConVarRef developerRef( "developer" );
	if ( cheatsRef.IsValid() )
	{
		cheatsRef->SetBool( true );
	}
	if ( developerRef.IsValid() )
	{
		developerRef->SetBool( true );
	}
#endif	// DEBUG

	// Add base search paths
	eastl::string exePath;
	S_GetFilePath( Sys_GetExecutablePath(), exePath, false );
	S_AppendPathSeparator( exePath );
	g_pFileSystem->AddSearchPath( exePath.c_str(), "enginebin" );
	g_pFileSystem->AddSearchPath( "core", "core" );

	// Initialize the profiler
#if ENABLE_PROFILING
	IProfiler* pProfiler = Profiler();
	pProfiler->Init();
	while ( CommandLine()->HasParam( "wait-profiler" ) && !pProfiler->IsConnected() )
	{
		Sys_Yield();
	}
#endif	// ENABLE_PROFILING
}

/*
==================
CApplication::Shutdown
==================
*/
void CApplication::Shutdown()
{
	// Shutdown the profiler
#if ENABLE_PROFILING
	Profiler()->Shutdown();
#endif	// ENABLE_PROFILING

	// Remove the log output file
#if ENABLE_LOGGING
	{
		CLogOutputFile& logOutputFile = GetLogOutputFile();
		logOutputFile.EndLoggingToFile();
		Logger()->RemoveOutput( &logOutputFile );
	}
#endif	// ENABLE_LOGGING

	// Unregister cvars
	ConVar_Unregister();
	DisconnectTier1();

	// Shutdown Tier1 and clear system groups array
	tier1SystemGroup.Shutdown();
	systemGroups.clear();

	// Shutdown Tier0
	ShutdownTier0();

	// Remove the log output stdout
#if ENABLE_LOGGING
	Logger()->RemoveOutput( &GetLogOutputStdOut() );
#endif	// ENABLE_LOGGING

	// Reset the current application for the factory
	s_pCurrentApp = NULL;
}

/*
==================
CApplication::AddGroup
==================
*/
void CApplication::AddGroup( CAppSystemGroup* pSystemGroup )
{
	// See if we already added it..
	Assert( pSystemGroup );
	for ( uint32 index = 0, count = (uint32)systemGroups.size(); index < count; ++index )
	{
		if ( systemGroups[index] == pSystemGroup )
		{
			return;
		}
	}

	// Otherwise add it
	pSystemGroup->Startup();
	systemGroups.emplace_back( pSystemGroup );
}

/*
==================
CApplication::RemoveGroup
==================
*/
void CApplication::RemoveGroup( CAppSystemGroup* pSystemGroup )
{
	Assert( pSystemGroup );
	for ( uint32 index = 0, count = (uint32)systemGroups.size(); index < count; ++index )
	{
		if ( systemGroups[index] == pSystemGroup )
		{
			pSystemGroup->Shutdown();
			systemGroups.erase( systemGroups.begin() + index );
			break;
		}
	}
}

/*
==================
CApplication::FindSystem
==================
*/
void* CApplication::FindSystem( const char* pInterfaceName ) const
{
	PROFILER_SCOPE_FUNC();
	for ( int32 index = (int32)systemGroups.size(); --index >= 0; )
	{
		void* pInterface = systemGroups[index]->FindSystem( pInterfaceName );
		if ( pInterface )
		{
			return pInterface;
		}
	}
	return NULL;
}

#if ENABLE_LOGGING
/*
==================
CApplication::GetLogOutputStdOut
==================
*/
CLogOutputStdOut& CApplication::GetLogOutputStdOut()
{
	static CLogOutputStdOut s_logOutputStdOut;
	return s_logOutputStdOut;
}

/*
==================
CApplication::GetLogOutputFile
==================
*/
CLogOutputFile& CApplication::GetLogOutputFile()
{
	static CLogOutputFile s_logOutputFile;
	return s_logOutputFile;
}
#endif	// ENABLE_LOGGING

/*
==================
CApplication::GetFactory
==================
*/
createInterfaceFn_t CApplication::GetFactory()
{
	return AppCreateInterfaceFn;
}
