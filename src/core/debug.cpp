#include "pch_core.h"
#include "core/crashdump_private.h"
#include "core/debug_private.h"

/*
 ==================
 DefaultLogOutput
 ==================
 */
static void DefaultLogOutput( const achar* pMsg )
{
#if ENABLE_LOGGING
	// Print message to OS console
	printf( pMsg );

	// Print message to debug output
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugMessage( pMsg );
	}
#endif // ENABLE_LOGGING
}

static logOutputFn_t	s_LogOutputFn	= &DefaultLogOutput;
logColor_t				g_LogColor		= LOG_COLOR_DEFAULT;

/*
==================
Sys_SetLogOutputFunc
==================
*/
void Sys_SetLogOutputFunc( logOutputFn_t pFunc )
{
	s_LogOutputFn = pFunc ? pFunc : Sys_GetDefaultLogOutput();
}

/*
==================
Sys_GetLogOutputFunc
==================
*/
logOutputFn_t Sys_GetLogOutputFunc()
{
	return s_LogOutputFn;
}

/*
==================
Sys_GetDefaultLogOutput
==================
*/
logOutputFn_t Sys_GetDefaultLogOutput()
{
	return &DefaultLogOutput;
}

/*
==================
Sys_ResetLogColor
==================
*/
void Sys_ResetLogColor()
{
	Sys_SetLogColor( LOG_COLOR_DEFAULT );
}

#if ENABLE_LOGGING
	/*
	==================
	Msg
	==================
	*/
	void Msg( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VMsg( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VMsg
	==================
	*/
	void VMsg( const achar* pFormat, va_list params )
	{
		bool	bIsAllowedChangeColor = g_LogColor == LOG_COLOR_DEFAULT;
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_WHITE );
		}
		s_LogOutputFn( S_Sprintf( "Msg: %s\n", S_Vsprintf( pFormat, params ).c_str() ).c_str() );
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_DEFAULT );
		}
	}

	/*
	==================
	Warning
	==================
	*/
	void Warning( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VWarning( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VWarning
	==================
	*/
	void VWarning( const achar* pFormat, va_list params )
	{
		bool	bIsAllowedChangeColor = g_LogColor == LOG_COLOR_DEFAULT;
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_YELLOW );
		}
		s_LogOutputFn( S_Sprintf( "Warning: %s\n", S_Vsprintf( pFormat, params ).c_str() ).c_str() );
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_DEFAULT );
		}
	}

	/*
	==================
	Error
	==================
	*/
	void Error( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VError( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VError
	==================
	*/
	void VError( const achar* pFormat, va_list params )
	{
		bool	bIsAllowedChangeColor = g_LogColor == LOG_COLOR_DEFAULT;
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_RED );
		}
		s_LogOutputFn( S_Sprintf( "Error: %s\n", S_Vsprintf( pFormat, params ).c_str() ).c_str() );
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_DEFAULT );
		}
	}
#endif // ENABLE_LOGGING

#if ENABLE_ASSERT
	/*
	==================
	Sys_FailAssertFunc
	==================
	*/
	void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat /*= "" */, ... )
	{
		// Don't show message if we already shutdown the game by a critical error
		static bool		bAlreadyHasError = false;
		if ( bAlreadyHasError )
		{
			return;
		}
		bAlreadyHasError = true;

		// Get final message
		va_list			params;
		va_start( params, pFormat );
		std::string		message = S_Sprintf( "Expression: %s\nMessage: %s\n\nFile: %s\nLine: %i", pExpr, S_Strlen( pFormat ) > 0 ? S_Vsprintf( pFormat, params ).c_str() : "<None>", pFile, line);
		va_end( params );

		// Print message and show message box
		bool	bIsAllowedChangeColor = g_LogColor == LOG_COLOR_DEFAULT;
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_RED );
		}
		s_LogOutputFn( "\n------------ ASSERTION FAILED --------------\n" );
		s_LogOutputFn( message.c_str() );
		s_LogOutputFn( "\n--------------------------------------------\n\n" );
		if ( bIsAllowedChangeColor )
		{
			Sys_SetLogColor( LOG_COLOR_DEFAULT );
		}

		if ( Sys_IsDebuggerPresent() )
		{
			Sys_DebugBreak();
		}
		Sys_ShowMessageBox( "Singularity Error", message.c_str(), MESSAGE_BOX_ERROR );

		// Set crash dump message
		CrashDump_SetMessage( message.c_str() );

		// Shutdown application
		Sys_RequestExit( true );
	}
#endif // ENABLE_ASSERT

#if ENABLE_ENSURE
	static bool		s_bEnsureAllowed = true;

	/*
	==================
	Sys_EnsureFunc
	==================
	*/
	void Sys_EnsureFunc( const achar* pExpr, const achar* pFile, int32 line, bool bAlways, const achar* pFormat /*= ""*/, ... )
	{
		if ( bAlways || s_bEnsureAllowed )
		{
			// Get final message
			va_list			params;
			va_start( params, pFormat );
			std::string		message = S_Sprintf( "Expression: %s\nMessage: %s\n\nFile: %s\nLine: %i", pExpr, S_Strlen( pFormat ) > 0 ? S_Vsprintf( pFormat, params ).c_str() : "<None>", pFile, line);
			va_end( params );

			// Print message and show message box
			bool	bIsAllowedChangeColor = g_LogColor == LOG_COLOR_DEFAULT;
			if ( bIsAllowedChangeColor )
			{
				Sys_SetLogColor( LOG_COLOR_RED );
			}
			s_LogOutputFn( "\n------------ ENSURE FAILED --------------\n" );
			s_LogOutputFn( message.c_str() );
			s_LogOutputFn( "\n--------------------------------------------\n\n" );
			if ( bIsAllowedChangeColor )
			{
				Sys_SetLogColor( LOG_COLOR_DEFAULT );
			}

			if ( Sys_IsDebuggerPresent() )
			{
				Sys_DebugBreak();
			}
		}
	}

	/*
	==================
	Sys_SetEnsureAllow
	==================
	*/
	void Sys_SetEnsureAllow( bool bAllowed )
	{
		s_bEnsureAllowed = bAllowed;
	}
#endif // ENABLE_ENSURE