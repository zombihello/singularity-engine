#pragma once
#include "core/coreapi.h"
#include "stdlib/types.h"
#include "core/memory.h"
#include "stdlib/template.h"
#include "stdlib/strtools.h"
#include "stdlib/guid.h"

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
CORE_INTERFACE void* Sys_CreateProc( const char* pPathToProcess, const char* pParams, bool bLaunchDetached, bool bLaunchHidden, int32 priorityModifier, uint64* pProcessId = nullptr );
CORE_INTERFACE bool	 Sys_GetProcReturnCode( void* pProcHandle, int32* pReturnCode );
CORE_INTERFACE bool	 Sys_IsProcRunning( void* pProcHandle );
CORE_INTERFACE void	 Sys_WaitForProc( void* pProcHandle );
CORE_INTERFACE void	 Sys_TerminateProc( void* pProcHandle );

// Functions to work with DLLs
CORE_INTERFACE dllHandle_t Sys_DLL_LoadModule( const char* pDLLName );
CORE_INTERFACE void		   Sys_DLL_UnloadModule( dllHandle_t pDLLHandle );
CORE_INTERFACE void*	   Sys_DLL_GetProcAddress( dllHandle_t pDLLHandle, const char* pFuncName );

// Indicates that MainLoop should be exited at the end of the current iteration
CORE_INTERFACE void Sys_RequestExit( bool bForce );
CORE_INTERFACE bool Sys_IsRequestingExit();

// Print critical error and to shutdown application
CORE_INTERFACE void			Sys_Error( const char* pFormat, ... );
CORE_INTERFACE void			Sys_ShowMessageBox( const char* pTitle, const char* pMessage, messageBox_t type );
CORE_INTERFACE const char* Sys_GetComputerName();
CORE_INTERFACE const char* Sys_GetUserName();
CORE_INTERFACE const char* Sys_GetExecutablePath();
CORE_INTERFACE double		Sys_Seconds();
CORE_INTERFACE void			Sys_InitGuid( CGuid& guid );
