#pragma once
#include "tier0/tier0_api.h"
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/assert.h"
#include "tier0/ilogger.h"
#include "tier0/memory.h"

//-----------------------------------------------------------------------------
// Core functions
//-----------------------------------------------------------------------------
enum messageBox_t
{
	MESSAGE_BOX_INFO,
	MESSAGE_BOX_WARNING,
	MESSAGE_BOX_ERROR
};

enum procPriority_t
{
	PROC_PRIORITY_IDLE,
	PROC_PRIORITY_BELOW_NORMAL,
	PROC_PRIORITY_NORMAL,
	PROC_PRIORITY_ABOVE_NORMAL,
	PROC_PRIORITY_HIGH
};

// Functions to initialize the main thread
TIER0_INTERFACE void Sys_InitMainThread();
TIER0_INTERFACE bool Sys_IsInMainThread();

// Functions to work with a process
TIER0_INTERFACE procHandle_t Sys_CreateProc( const char* pPath, const char* pParams, bool bLaunchDetached, bool bLaunchHidden, procPriority_t priority = PROC_PRIORITY_NORMAL, uint64* pProcessId = nullptr );
TIER0_INTERFACE bool		 Sys_GetProcReturnCode( procHandle_t pProcHandle, int32* pReturnCode );
TIER0_INTERFACE bool		 Sys_IsProcRunning( procHandle_t pProcHandle );
TIER0_INTERFACE void		 Sys_WaitForProc( procHandle_t pProcHandle );
TIER0_INTERFACE void		 Sys_TerminateProc( procHandle_t pProcHandle );

// Functions to work with DLLs
TIER0_INTERFACE dllHandle_t Sys_DLL_LoadModule( const char* pDLLName );
TIER0_INTERFACE void		Sys_DLL_UnloadModule( dllHandle_t pDLLHandle );
TIER0_INTERFACE void*		Sys_DLL_GetProcAddress( dllHandle_t pDLLHandle, const char* pFuncName );

// Indicates that MainLoop should be exited at the end of the current iteration
TIER0_INTERFACE void Sys_RequestExit( bool bForce );
TIER0_INTERFACE bool Sys_IsRequestingExit();

// Print critical error and to shutdown application
TIER0_INTERFACE void		Sys_Error( const char* pFormat, ... );
TIER0_INTERFACE void		Sys_ShowMessageBox( const char* pTitle, const char* pMessage, messageBox_t type );
TIER0_INTERFACE const char* Sys_GetComputerName();
TIER0_INTERFACE const char* Sys_GetUserName();
TIER0_INTERFACE const char* Sys_GetExecutablePath();
TIER0_INTERFACE double		Sys_Seconds();
