/**
 * @file
 * @addtogroup core core
 */

#ifndef CORE_H
#define CORE_H

#include "core/build.h"
#include "core/coreapi.h"
#include "core/types.h"
#include "core/platform.h"
#include "core/memory.h"
#include "stdlib/template.h"
#include "stdlib/strtools.h"
#include "stdlib/guid.h"

/**
 * @ingroup core
 * @brief Message box type
 */
enum messageBox_t
{
    MESSAGE_BOX_INFO,        /**< Info */
    MESSAGE_BOX_WARNING,     /**< Warning */
    MESSAGE_BOX_ERROR        /**< Error */
};

/**
 * @ingroup core
 * @brief Macro for mark deprecated code
 *
 * @param Version	Engine version where this code will be removed
 * @param Message	Custom message
 *
 * Example usage:
 * @code
 * DEPRECATED( 1.0.0, "Your message" )
 * void Function()
 * {}
 * @endcode
 */
#define DEPRECATED( Version, Message )			        [ [ deprecated( Message " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile." ) ] ]

/**
 * @ingroup core
 * @brief Macro for empty code
 */
#define NOOP					( void )0

/**
 * @ingroup core
 * @brief Macro for labeling a variable that it is not used
 * 
 * @param Var	Variable
 * 
 * Example usage: @code UNUSED_VAR( health ); @endcode
 */
#define UNUSED_VAR( Var )     ( void )( Var )

/**
 * @ingroup core
 * @brief Macro for concatenation of two values
 *
 * @param X	    Value X
 * @param Y     Value Y
 *
 * Example usage: @code CONCAT( My, __LINE__ ); @endcode
 */
#define CONCAT_IMPL( X, Y ) X##Y
#define CONCAT( X, Y )      CONCAT_IMPL( X, Y )

/**
 * @ingroup core
 * @brief Print critical error and to shutdown application
 *
 * @param pFormat	Display message format
 * @param ...		Message parameters
 *
 * Example usage: @code Sys_Error( "Hello Error %i", 43 ); @endcode
 */
CORE_INTERFACE void Sys_Error( const achar* pFormat, ... );

/**
 * @ingroup core
 * @brief Create process
 * @note Need implement on each platform
 * 
 * @param pPathToProcess        Executable name
 * @param pParams               Command line arguments
 * @param bLaunchDetached       If TRUE, the new process will have its own window
 * @param bLaunchHidden         If TRUE, the new process will not have a window or be in the task bar
 * @param priorityModifier      -2 idle, -1 low, 0 normal, 1 high, 2 higher
 * @param pProcessId            If non-NULL, this will be filled in with the ProcessId
 * @return The process handle for use in other process functions
 */
CORE_INTERFACE void* Sys_CreateProc( const achar* pPathToProcess, const achar* pParams, bool bLaunchDetached, bool bLaunchHidden, int32 priorityModifier, uint64* pProcessId = nullptr );

/**
 * @ingroup core
 * @brief Retrieves the termination status of the specified process
 * @note Need implement on each platform
 * 
 * @param pProcHandle   Handle of process returned from Sys_CreateProc
 * @param pReturnCode   Return code
 * @return Return TRUE if process is end, otherwise returns FALSE
 */
CORE_INTERFACE bool Sys_GetProcReturnCode( void* pProcHandle, int32* pReturnCode );

/**
 * @ingroup core
 * @brief Is process running
 * @note Need implement on each platform
 * 
 * @param pProcHandle   Handle of process returned from Sys_CreateProc
 * @return Return TRUE if process is running, otherwise returns FALSE
 */
CORE_INTERFACE bool Sys_IsProcRunning( void* pProcHandle );

/**
 * @ingroup core
 * @brief Waits for a process to stop
 * @note Need implement on each platform
 * 
 * @param pProcHandle   Handle of process returned from Sys_CreateProc
 */
CORE_INTERFACE void Sys_WaitForProc( void* pProcHandle );

/**
 * @ingroup core
 * @brief Terminates a process
 * @note Need implement on each platform
 * 
 * @param pProcHandle   Handle of process returned from Sys_CreateProc
 */
CORE_INTERFACE void Sys_TerminateProc( void* pProcHandle );

/**
 * @ingroup core
 * @brief Show message box
 * @note Need implement on each platform
 *
 * @param pTitle    Title
 * @param pMessage  Message
 * @param type      Message type
 */
CORE_INTERFACE void Sys_ShowMessageBox( const achar* pTitle, const achar* pMessage, messageBox_t type );

/**
 * @ingroup core
 * @brief Load DLL module
 * @note Need implement on each platform
 *
 * @param pDLLName    DLL name
 * @return Return DLL handle, if it is not loaded returns NULL
 */
CORE_INTERFACE dllHandle_t Sys_DLL_LoadModule( const achar* pDLLName );

/**
 * @ingroup core
 * @brief Unload DLL module
 * @note Need implement on each platform
 *
 * @param pDLLHandle   DLL handle
 */
CORE_INTERFACE void Sys_DLL_UnloadModule( dllHandle_t pDLLHandle );

/**
 * @ingroup core
 * @brief Get address to function in DLL module
 * @note Need implement on each platform
 *
 * @param pDLLHandle  DLL handle
 * @param pFuncName   Function name
 * @return Return pointer to function in DLL module. If the one isn't in the module returns NULL
 */
CORE_INTERFACE void* Sys_DLL_GetProcAddress( dllHandle_t pDLLHandle, const achar* pFuncName );

/**
 * @ingroup core
 * @brief Request shutdown application
 * @note Need implement on each platform
 * 
 * @param bForce   Is need force shutdown application
 */
CORE_INTERFACE void Sys_RequestExit( bool bForce );

/**
 * @ingroup core
 * @brief Indicates that MainLoop should be exited at the end of the current iteration
 * 
 * @return Return TRUE when MainLoop should be exited at the end of the current iteration, otherwise FALSE
 */
CORE_INTERFACE bool Sys_IsRequestingExit();

/**
 * @ingroup core
 * @brief Get computer name
 * @note Need implement on each platform
 *
 * @return Return computer name
 */
CORE_INTERFACE const achar* Sys_GetComputerName();

/**
 * @ingroup core
 * @brief Get user name
 * @note Need implement on each platform
 *
 * @return Return user name
 */
CORE_INTERFACE const achar* Sys_GetUserName();

/**
 * @ingroup core
 * @brief Get absolute path to the current executable module
 * @note Need implement on each platform
 *
 * This function returns absolute path to the current executable module. 
 * For example: C:/game/bin/win64/episodic.exe
 * 
 * @return Return absolute path to the current executable module
 */
CORE_INTERFACE const achar* Sys_GetExecutablePath();

/**
 * @ingroup core
 * @brief Get time in seconds
 * @note Need implement on each platform
 * 
 * @reutrn Return time in seconds
 */
CORE_INTERFACE double Sys_Seconds();

/**
 * @ingroup core
 * @brief Initialize a guid
 * @note Need implement on each platform
 * 
 * @param guid  Guid to initialize
 */
CORE_INTERFACE void Sys_InitGuid( CGuid& guid );

#endif // !CORE_H