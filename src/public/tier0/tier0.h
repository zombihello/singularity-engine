#pragma once
#include "tier0/tier0_api.h"
#include "tier1/types.h"
#include "tier0/memory.h"
#include "tier1/template.h"
#include "tier1/strtools.h"
#include "tier1/guid.h"

//-----------------------------------------------------------------------------
// Core functions
//-----------------------------------------------------------------------------
enum messageBox_t
{
	MESSAGE_BOX_INFO,
	MESSAGE_BOX_WARNING,
	MESSAGE_BOX_ERROR
};

// Functions to work with a process
TIER0_INTERFACE void* Sys_CreateProc( const char* pPathToProcess, const char* pParams, bool bLaunchDetached, bool bLaunchHidden, int32 priorityModifier, uint64* pProcessId = nullptr );
TIER0_INTERFACE bool  Sys_GetProcReturnCode( void* pProcHandle, int32* pReturnCode );
TIER0_INTERFACE bool  Sys_IsProcRunning( void* pProcHandle );
TIER0_INTERFACE void  Sys_WaitForProc( void* pProcHandle );
TIER0_INTERFACE void  Sys_TerminateProc( void* pProcHandle );

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
TIER0_INTERFACE void		Sys_InitGuid( CGuid& guid );
