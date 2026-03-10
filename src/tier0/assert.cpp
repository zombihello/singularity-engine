#include "pch_tier0.h"
#include "tier0/icrashdump_handler.h"
#include "tier0/assert.h"
#include "tier0/debug.h"

#if ENABLE_ASSERT
/*
==================
Sys_AssertFailed
==================
*/
bool Sys_AssertFailed( const char* pExpr, const char* pFile, int32 line, const char* pFormat /*= "" */, ... )
{
	// Don't show message if we already shutdown the game by a critical error
	static bool s_bAlreadyHasError = false;
	if ( s_bAlreadyHasError )
	{
		return false;
	}
	s_bAlreadyHasError = true;

	// Get the message
	va_list params;
	va_start( params, pFormat );
	eastl::string message = S_Strlen( pFormat ) > 0 ? S_Vsprintf( pFormat, params ) : "<None>";
	va_end( params );

	// Print the message and show message box
	Error( "------------ ASSERTION FAILED --------------" );
	Error( "Expression: %s", pExpr );
	Error( "Message: %s", message.c_str() );
	Error( "" );
	Error( "File: %s", pFile );
	Error( "Line: %i", line );
	Error( "--------------------------------------------" );

	Sys_DebugBreak();
	eastl::string fullMessage = S_Sprintf( "Expression: %s\nMessage: %s\n\nFile: %s\nLine: %i", pExpr, message.c_str(), pFile, line );
	Sys_ShowMessageBox( "Singularity Error", fullMessage.c_str(), MESSAGE_BOX_ERROR );

	// Set crash dump message
	CrashDumpHandler()->SetMessage( fullMessage.c_str() );

	// Shutdown application
	Sys_RequestExit( true );
	return true;
}
#endif	// ENABLE_ASSERT

#if ENABLE_ENSURE
static bool s_bEnsureAllowed = true;

/*
==================
Sys_EnsureFailed
==================
*/
bool Sys_EnsureFailed( const char* pExpr, const char* pFile, int32 line, bool bAlways, const char* pFormat /*= ""*/, ... )
{
	if ( bAlways || s_bEnsureAllowed )
	{
		// Get the final message
		va_list params;
		va_start( params, pFormat );
		eastl::string message = S_Strlen( pFormat ) > 0 ? S_Vsprintf( pFormat, params ) : "<None>";
		va_end( params );

		// Print the message
		Error( "------------ ENSURE FAILED --------------" );
		Error( "Expression: %s", pExpr );
		Error( "Message: %s", message.c_str() );
		Error( "" );
		Error( "File: %s", pFile );
		Error( "Line: %i", line );
		Error( "--------------------------------------------" );
		Sys_DebugBreak();
		return true;
	}

	return false;
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
#endif	// ENABLE_ENSURE
