#include "pch_tier0.h"
#include <combaseapi.h>

/*
==================
Sys_CreateProc
==================
*/
procHandle_t Sys_CreateProc( const char* pPath, const char* pParams, bool bLaunchDetached, bool bLaunchHidden, procPriority_t priority /* = PROC_PRIORITY_NORMAL */, uint64* pProcessId /* = NULL */ )
{
	eastl::wstring		commandLine = UTF8_TO_WCHAR( S_Sprintf( "%s %s", pPath, pParams ).c_str() );
	PROCESS_INFORMATION procInfo;
	SECURITY_ATTRIBUTES attributes;
	attributes.nLength				= sizeof( SECURITY_ATTRIBUTES );
	attributes.lpSecurityDescriptor = NULL;
	attributes.bInheritHandle		= true;
	if ( pProcessId )
	{
		*pProcessId = 0;
	}

	uint64 createFlags = 0;
	switch ( priority )
	{
	case PROC_PRIORITY_IDLE: createFlags |= IDLE_PRIORITY_CLASS; break;
	case PROC_PRIORITY_BELOW_NORMAL: createFlags |= BELOW_NORMAL_PRIORITY_CLASS; break;
	case PROC_PRIORITY_NORMAL: createFlags |= NORMAL_PRIORITY_CLASS; break;
	case PROC_PRIORITY_ABOVE_NORMAL: createFlags |= ABOVE_NORMAL_PRIORITY_CLASS; break;
	case PROC_PRIORITY_HIGH: createFlags |= HIGH_PRIORITY_CLASS; break;
	default:
		AssertMsg( false, "Unknown process priority 0x%X", priority );
		createFlags |= NORMAL_PRIORITY_CLASS;
		break;
	}

	if ( bLaunchDetached )
	{
		createFlags |= DETACHED_PROCESS;
	}

	uint64 flags		   = 0;
	uint32 showWindowFlags = SW_HIDE;
	if ( bLaunchHidden )
	{
		flags			= STARTF_USESHOWWINDOW;
		showWindowFlags = SW_HIDE;
		if ( bLaunchDetached )	// If hiding the window, and running detached, create a new console
		{
			createFlags = CREATE_NEW_CONSOLE;
		}
	}

	STARTUPINFOW startupInfo = {
		sizeof( STARTUPINFOW ), NULL, NULL, NULL,
		(DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
		NULL, NULL, NULL, (DWORD)flags,
		(WORD)showWindowFlags, NULL, NULL, NULL,
		NULL, NULL
	};

	eastl::wstring currentDirectory;
	currentDirectory.resize( GetCurrentDirectoryW( 0, NULL ) );
	uint32 length = GetCurrentDirectoryW( (uint32)currentDirectory.size(), currentDirectory.data() );
	AssertMsg( length > 0, "Failed to get current directory (GetLastError 0x%X)", GetLastError() );

	if ( !CreateProcessW( NULL, (LPWSTR)commandLine.c_str(), &attributes, &attributes, TRUE, (DWORD)createFlags, NULL, (LPCWSTR)currentDirectory.c_str(), (LPSTARTUPINFOW)&startupInfo, &procInfo ) )
	{
		return NULL;
	}

	if ( pProcessId )
	{
		*pProcessId = procInfo.dwProcessId;
	}
	return (procHandle_t)procInfo.hProcess;
}

/*
==================
Sys_GetProcReturnCode
==================
*/
bool Sys_GetProcReturnCode( procHandle_t pProcHandle, int32* pReturnCode )
{
	return GetExitCodeProcess( (HANDLE)pProcHandle, (DWORD*)pReturnCode ) && *( (DWORD*)pReturnCode ) != STILL_ACTIVE;
}

/*
==================
Sys_IsProcRunning
==================
*/
bool Sys_IsProcRunning( procHandle_t pProcHandle )
{
	DWORD waitResult = WaitForSingleObject( (HANDLE)pProcHandle, 0 );
	return waitResult != WAIT_TIMEOUT ? false : true;
}

/*
==================
Sys_WaitForProc
==================
*/
void Sys_WaitForProc( procHandle_t pProcHandle )
{
	WaitForSingleObject( (HANDLE)pProcHandle, INFINITE );
}

/*
==================
Sys_TerminateProc
==================
*/
void Sys_TerminateProc( procHandle_t pProcHandle )
{
	TerminateProcess( (HANDLE)pProcHandle, 0 );
}

/*
==================
Sys_DLL_LoadModule
==================
*/
dllHandle_t Sys_DLL_LoadModule( const char* pDLLName )
{
	return LoadLibraryExW( UTF8_TO_WCHAR( pDLLName ), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
}

/*
==================
Sys_DLL_UnloadModule
==================
*/
void Sys_DLL_UnloadModule( dllHandle_t pDLLHandle )
{
	if ( pDLLHandle )
	{
		FreeLibrary( pDLLHandle );
	}
}

/*
==================
Sys_DLL_GetProcAddress
==================
*/
void* Sys_DLL_GetProcAddress( dllHandle_t pDLLHandle, const char* pFuncName )
{
	if ( pDLLHandle )
	{
		return GetProcAddress( pDLLHandle, pFuncName );
	}
	return NULL;
}

/*
==================
Sys_RequestExit
==================
*/
void Sys_RequestExit( bool bForce )
{
	if ( bForce )
	{
		// Force immediate exit
		terminate();
	}
	else
	{
		// Tell the platform specific code we want to exit cleanly from the main loop.
		PostQuitMessage( 0 );
		g_bRequestingExit = true;
	}
}

/*
==================
Sys_GetComputerName
==================
*/
const char* Sys_GetComputerName()
{
	static eastl::string s_computerName;
	if ( s_computerName.empty() )
	{
		// Get length of the computer name
		DWORD size = 0;
		GetComputerNameW( NULL, &size );

		// Get the computer name
		eastl::wstring wideComputerName;
		wideComputerName.resize( size );
		BOOL bResult = GetComputerNameW( wideComputerName.data(), &size );
		Assert( bResult );

		// Convert WCHAR to UTF8 string
		s_computerName = WCHAR_TO_UTF8( wideComputerName.c_str() );
	}

	return s_computerName.c_str();
}

/*
==================
Sys_GetUserName
==================
*/
const char* Sys_GetUserName()
{
	static eastl::string s_userName;
	if ( s_userName.empty() )
	{
		// Get length of the user name
		DWORD size = 0;
		GetUserNameW( NULL, &size );

		// Get the user name
		eastl::wstring wideUserName;
		wideUserName.resize( size );
		BOOL bResult = GetUserNameW( wideUserName.data(), &size );
		Assert( bResult );

		// Convert WCHAR to UTF8 string
		s_userName = WCHAR_TO_UTF8( wideUserName.c_str() );
	}

	return s_userName.c_str();
}

/*
==================
Sys_GetExecutablePath
==================
*/
const char* Sys_GetExecutablePath()
{
	static eastl::string s_executablePath;
	if ( s_executablePath.empty() )
	{
		DWORD		   size = MAX_PATH;
		eastl::wstring wideBuffer;
		while ( true )
		{
			// Try get an executable path
			wideBuffer.resize( size );
			DWORD length = GetModuleFileNameW( NULL, wideBuffer.data(), size );
			AssertMsg( length > 0, "Failed to get executable path (GetLastError 0x%X)", GetLastError() );

			// Done if the path fits into the buffer
			if ( length < size - 1 )
			{
				break;
			}

			// Otherwise we increase the buffer size
			size *= 2;
		}

		// Convert WCHAR to UTF8 string
		s_executablePath = WCHAR_TO_UTF8( wideBuffer.c_str() );
	}

	return s_executablePath.c_str();
}

/*
==================
Sys_Seconds
==================
*/
double Sys_Seconds()
{
	LARGE_INTEGER cycles;
	QueryPerformanceCounter( &cycles );

	// Add big number to make bugs apparent where return value is being passed to FLOAT
	return cycles.QuadPart * g_SecondsPerCycle + 16777216.0;
}

/*
==================
Sys_GetSecondsPerCycle
==================
*/
double Sys_GetSecondsPerCycle()
{
	LARGE_INTEGER frequency;
	bool		  bResult = QueryPerformanceFrequency( &frequency );

	Assert( bResult );
	return 1.0 / frequency.QuadPart;
}
