#pragma once

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
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_MESSAGE, pFormat, params );
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
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_WARNING, pFormat, params );
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
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_ERROR, pFormat, params );
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
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_MESSAGE, pFormat, params );
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
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_WARNING, pFormat, params );
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
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_ERROR, pFormat, params );
	va_end( params );
#endif	// ENABLE_LOGGING
}
