#include "pch_tier0.h"
#include <combaseapi.h>

/*
==================
Sys_CreateProc
==================
*/
void* Sys_CreateProc( const char* pPathToProcess, const char* pParams, bool bLaunchDetached, bool bLaunchHidden, int32 priorityModifier, uint64* pProcessId /* = NULL */ )
{
	eastl::string		commandLine = S_Sprintf( "%s %s", pPathToProcess, pParams );
	PROCESS_INFORMATION procInfo;
	SECURITY_ATTRIBUTES attributes;
	attributes.nLength				= sizeof( SECURITY_ATTRIBUTES );
	attributes.lpSecurityDescriptor = nullptr;
	attributes.bInheritHandle		= true;

	uint64 createFlags = NORMAL_PRIORITY_CLASS;
	if ( priorityModifier < 0 )
	{
		if ( priorityModifier == -1 )
		{
			createFlags = BELOW_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = IDLE_PRIORITY_CLASS;
		}
	}
	else if ( priorityModifier > 0 )
	{
		if ( priorityModifier == 1 )
		{
			createFlags = ABOVE_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = HIGH_PRIORITY_CLASS;
		}
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

	STARTUPINFO startupInfo = {
		sizeof( STARTUPINFO ), NULL, NULL, NULL,
		(DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
		NULL, NULL, NULL, (DWORD)flags,
		(WORD)showWindowFlags, NULL, NULL, NULL,
		NULL, NULL
	};

	char path[MAX_PATH];
	GetCurrentDirectoryA( MAX_PATH, path );

	if ( !CreateProcessA( NULL, (LPSTR)commandLine.c_str(), &attributes, &attributes, TRUE, (DWORD)createFlags, NULL, (LPCSTR)path, (LPSTARTUPINFOA)&startupInfo, &procInfo ) )
	{
		if ( pProcessId )
		{
			*pProcessId = 0;
		}

		return NULL;
	}

	if ( pProcessId )
	{
		*pProcessId = procInfo.dwProcessId;
	}
	return (void*)procInfo.hProcess;
}

/*
==================
Sys_GetProcReturnCode
==================
*/
bool Sys_GetProcReturnCode( void* pProcHandle, int32* pReturnCode )
{
	return GetExitCodeProcess( (HANDLE)pProcHandle, (DWORD*)pReturnCode ) && *( (DWORD*)pReturnCode ) != STILL_ACTIVE;
}

/*
==================
Sys_IsProcRunning
==================
*/
bool Sys_IsProcRunning( void* pProcHandle )
{
	DWORD waitResult = WaitForSingleObject( (HANDLE)pProcHandle, 0 );
	return waitResult != WAIT_TIMEOUT ? false : true;
}

/*
==================
Sys_WaitForProc
==================
*/
void Sys_WaitForProc( void* pProcHandle )
{
	WaitForSingleObject( (HANDLE)pProcHandle, INFINITE );
}

/*
==================
Sys_TerminateProc
==================
*/
void Sys_TerminateProc( void* pProcHandle )
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
	return LoadLibraryExA( pDLLName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
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
	static char result[256] = "";
	if ( !result[0] )
	{
		DWORD size = ARRAYSIZE( result );
		GetComputerNameA( result, &size );
	}
	return result;
}

/*
==================
Sys_GetUserName
==================
*/
const char* Sys_GetUserName()
{
	static char result[256] = "";
	if ( !result[0] )
	{
		DWORD size = ARRAYSIZE( result );
		GetUserNameA( result, &size );
	}
	return result;
}

/*
==================
Sys_GetExecutablePath
==================
*/
const char* Sys_GetExecutablePath()
{
	static char path[MAX_PATH] = "";
	if ( !path[0] )
	{
		GetModuleFileNameA( NULL, path, MAX_PATH );
	}
	return path;
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
