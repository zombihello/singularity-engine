#pragma once

/*
==================
CLogOutputStdOut::CLogOutputStdOut
==================
*/
FORCEINLINE CLogOutputStdOut::CLogOutputStdOut()
{
	Sys_InitConsoleColorContext( colorContext );
}

/*
==================
CLogOutputStdOut::Print
==================
*/
FORCEINLINE void CLogOutputStdOut::Print( const logContext_t& context, const char* pMessage )
{
	uint16 prevColor = -1;
	if ( prevColor != UNSPECIFIED_LOG_COLOR )
	{
		prevColor = Sys_SetConsoleColor( colorContext, context.color );
	}

#if PLATFORM_USE_WCHAR_STDOUT
	wprintf( UTF8_TO_WCHAR( pMessage ) );
#else
	printf( pMessage );
#endif	// PLATFORM_USE_WCHAR_STDOUT

	if ( prevColor >= 0 )
	{
		Sys_RestoreConsoleColor( colorContext, prevColor );
	}
}

/*
==================
Msg
==================
*/
FORCEINLINE void Msg( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_MESSAGE, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
Msg
==================
*/
FORCEINLINE void Msg( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_MESSAGE, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
Warning
==================
*/
FORCEINLINE void Warning( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_WARNING, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
Warning
==================
*/
FORCEINLINE void Warning( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_WARNING, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
Error
==================
*/
FORCEINLINE void Error( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_ERROR, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
Error
==================
*/
FORCEINLINE void Error( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_ERROR, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevMsg
==================
*/
FORCEINLINE void DevMsg( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_MESSAGE, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevMsg
==================
*/
FORCEINLINE void DevMsg( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_MESSAGE, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevWarning
==================
*/
FORCEINLINE void DevWarning( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_WARNING, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevWarning
==================
*/
FORCEINLINE void DevWarning( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_WARNING, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevError
==================
*/
FORCEINLINE void DevError( const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_ERROR, UNSPECIFIED_LOG_COLOR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}

/*
==================
DevError
==================
*/
FORCEINLINE void DevError( const CColor& color, const char* pFormat, ... )
{
#if ENABLE_LOGGING
	va_list params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_ERROR, color, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}
