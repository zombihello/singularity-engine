/**
 * @file
 * @addtogroup core core
 */

#ifndef WIN_PLATFORM_H
#define WIN_PLATFORM_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>

// Undo any defines
#undef PLATFORM_WINDOWS
#undef VARARGS
#undef CDECL
#undef STDCALL
#undef FORCEINLINE
#undef FORCENOINLINE
#undef DLLEXPORT
#undef DLLIMPORT
#undef FALSE
#undef TRUE
#undef NULL

// Mark what we on Windows
#define PLATFORM_WINDOWS	1

#if RETAIL
    #define Sys_IsDebuggerPresent()	                false
    #define Sys_DebugBreak()
    #define Sys_DebugMessage( Msg )
#else
    /**
     * @ingroup core
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define RETAIL this macro is always return FALSE
     */
    #define Sys_IsDebuggerPresent()                 IsDebuggerPresent()

    /**
    * @ingroup core
    * @brief Macro for for triggering breakpoint
    * @warning With enabled define RETAIL this macro is empty
    */
    #define Sys_DebugBreak()			            ( Sys_IsDebuggerPresent() ? ( DebugBreak(), 1 ) : 1 )

    /**
     * @ingroup core
     * @brief Macro for print message to debugger
     * @warning With enabled define RETAIL this macro is empty
     * 
     * @param Msg	    Message
     */
    #define Sys_DebugMessage( Msg )			        OutputDebugStringA( Msg )
#endif // RETAIL

/**
 * @ingroup core
 * @brief DLL extension
 */
#define DLL_EXT_STRING			        ".dll"

// If macro PLATFORM_SUBDIR not defined while compiling, we define it now!
#ifndef PLATFORM_SUBDIR
	#if PLATFORM_64BIT
		/**
		 * @ingroup core
		 * @brief Platform sub directory for binaries
		 */
		#define PLATFORM_SUBDIR		    "win64"
	#elif PLATFORM_32BIT
		#define PLATFORM_SUBDIR		    "win32"
	#else
		#error Unknown platform bit
	#endif // PLATFORM_64BIT || PLATFORM_DOXYGEN
#endif // PLATFORM_SUBDIR

/**
 * @ingroup core
 * @brief Calling convention. Functions with variable arguments
 */
#define VARARGS				            __cdecl

/**
 * @ingroup core
 * @brief Calling convention. Standard C function
 */
#define CDECL				            __cdecl

/**
 * @ingroup core
 * @brief Calling convention. Standard calling convention
 */
#define STDCALL				            __stdcall

/**
 * @ingroup core
 * @brief Force code to be inline
 */
#define FORCEINLINE			            __forceinline

/**
 * @ingroup core
 * @brief Force code to NOT be inline
 */
#define FORCENOINLINE		            __declspec( noinline )

/**
 * @ingroup core
 * @brief Used for DLL exporting
 */
#define DLL_EXPORT				        __declspec( dllexport )

/**
 * @ingroup core
 * @brief Used for DLL importing
 */
#define DLL_IMPORT				        __declspec( dllimport )

/**
 * @ingroup core
 * @brief Function signature
 */
#define FUNCSIG                       __FUNCSIG__

/**
 * @ingroup core
 * @brief True macro
 */
#define TRUE				            1

/**
 * @ingroup core
 * @brief False macro
 */
#define FALSE				            0

/**
 * @ingroup core
 * @brief Null macro
 */
#define NULL				            0

/**
 * @ingroup core
 * @brief Line terminator
 */
#define LINE_TERMINATOR                 '\n'

/**
 * @ingroup core
 * @brief Path separator
 */
#define PATH_SEPARATOR                  '\\'
 
/**
 * @ingroup core
 * @brief Align for GCC
 *
 * @param Alignment     Alignment
 */
#define GCC_ALIGN( Alignment )

/**
 * @ingroup core
 * @brief Align for Microsoft
 * 
 * @param Alignment     Alignment
 */
#define MS_ALIGN( Alignment )           __declspec( align( Alignment ) )

/**
 * @ingroup core
 * @brief Invalid DLL handle
 */
#define INVALID_DLL_HANDLE 			    NULL

/**
 * @ingroup core
 * @brief Invalid thread handle
 */
#define INVALID_THREAD_HANDLE           NULL

/**
 * @ingroup core
 * @brief Invalid thread id
 */
#define INVALID_THREAD_ID               0

/**
 * @ingroup core
 * @brief Invalid window handle
 */
#define INVALID_WINDOW_HANDLE 		    NULL

/**
 * @ingroup core
 * @brief DLL handle
 */
typedef	HMODULE	                        dllHandle_t;

/**
 * @ingroup core
 * @brief Window handle
 */
typedef HANDLE                          windowHandle_t;

/**
 * @ingroup core
 * @brief Application instance handle
 */
typedef HINSTANCE                       appInstanceHandle_t;

/**
 * @ingroup core
 * @brief Thread handle
 */
typedef HANDLE                          threadHandle_t;

/**
 * @ingroup core
 * @brief Thread id
 */
typedef DWORD                           threadId_t;

#endif // !WIN_PLATFORM_H