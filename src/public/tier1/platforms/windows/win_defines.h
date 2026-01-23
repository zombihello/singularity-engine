#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <direct.h>

//-----------------------------------------------------------------------------
// Undo any defines
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Platform specific defines
//-----------------------------------------------------------------------------
#define PLATFORM_WINDOWS 1

#if RETAIL
	#define Sys_IsDebuggerPresent() false
	#define Sys_DebugBreak()
	#define Sys_DebugMessage( Msg )
#else
	// Macro for checking the presence of a debugger
	#define Sys_IsDebuggerPresent() IsDebuggerPresent()

	// Macro for for triggering breakpoint
	#define Sys_DebugBreak()		( Sys_IsDebuggerPresent() ? ( DebugBreak(), 1 ) : 1 )

	// Macro for print message to debugger
	#define Sys_DebugMessage( Msg ) OutputDebugStringA( Msg )
#endif	// RETAIL

#define DLL_EXT_STRING		   ".dll"
#define EXE_EXT_STRING		   ".exe"
#define VARARGS				   __cdecl
#define CDECL				   __cdecl
#define STDCALL				   __stdcall
#define FORCEINLINE			   __forceinline
#define FORCENOINLINE		   __declspec( noinline )
#define DLL_EXPORT			   __declspec( dllexport )
#define DLL_IMPORT			   __declspec( dllimport )
#define FUNCSIG				   __FUNCSIG__
#define TRUE				   1
#define FALSE				   0
#define NULL				   0
#define LINE_TERMINATOR		   '\n'
#define LINE_TERMINATOR_STRING "\n"
#define PATH_SEPARATOR		   '\\'
#define PATH_SEPARATOR_STRING  "\\"
#define GCC_ALIGN( Alignment )
#define MS_ALIGN( Alignment ) __declspec( align( Alignment ) )

#define INVALID_DLL_HANDLE	  NULL
#define INVALID_THREAD_HANDLE NULL
#define INVALID_THREAD_ID	  0
#define INVALID_WINDOW_HANDLE NULL

//-----------------------------------------------------------------------------
// Platform specific types
//-----------------------------------------------------------------------------
typedef HMODULE	  dllHandle_t;
typedef HANDLE	  windowHandle_t;
typedef HINSTANCE appInstanceHandle_t;
typedef HANDLE	  threadHandle_t;
typedef DWORD	  threadId_t;
