#include "pch_tier0.h"
#include "tier0/consoleio.h"

/*
==================
Sys_SetupConsoleIO
==================
*/
void Sys_SetupConsoleIO()
{
	// Ensure the handle associated with stdio is FILE_TYPE_UNKNOWND
	if ( GetFileType( GetStdHandle( STD_OUTPUT_HANDLE ) ) != FILE_TYPE_UNKNOWN )
	{
		return;
	}

	// Try to attach the parent console, if not allocate a new console
	if ( !AttachConsole( ATTACH_PARENT_PROCESS ) )
	{
		AllocConsole();
	}

	// Setup stdin, stdout and stderr to read and write to and from the console
	freopen( "conin$", "r", stdin );
	freopen( "conout$", "w", stdout );
	freopen( "conout$", "w", stderr );
}

/*
==================
Sys_InitConsoleColorContext
==================
*/
void Sys_InitConsoleColorContext( consoleColorContext_t& context )
{
	// Get the old background attributes
	HANDLE stdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	if ( GetFileType( stdHandle ) == FILE_TYPE_UNKNOWN )
	{
		return;
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	GetConsoleScreenBufferInfo( stdHandle, &consoleScreenBufferInfo );
	context.initialColor	= consoleScreenBufferInfo.wAttributes & ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY );
	context.backgroundColor = consoleScreenBufferInfo.wAttributes & ( BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY );
	context.lastColor		= context.initialColor;
	context.badColor		= 0;

	if ( context.backgroundColor & BACKGROUND_RED )
	{
		context.badColor |= FOREGROUND_RED;
	}
	if ( context.backgroundColor & BACKGROUND_GREEN )
	{
		context.badColor |= FOREGROUND_GREEN;
	}
	if ( context.backgroundColor & BACKGROUND_BLUE )
	{
		context.badColor |= FOREGROUND_BLUE;
	}
	if ( context.backgroundColor & BACKGROUND_INTENSITY )
	{
		context.badColor |= FOREGROUND_INTENSITY;
	}
}

/*
==================
Sys_SetConsoleColor
==================
*/
uint16 Sys_SetConsoleColor( consoleColorContext_t& context, const CColor& color )
{
	HANDLE stdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	if ( GetFileType( stdHandle ) == FILE_TYPE_UNKNOWN )
	{
		return 0;
	}

	uint16 prevColor  = context.lastColor;
	context.lastColor = 0;
	if ( color.r > 0 )
	{
		context.lastColor |= FOREGROUND_RED;
	}
	if ( color.g > 0 )
	{
		context.lastColor |= FOREGROUND_GREEN;
	}
	if ( color.b > 0 )
	{
		context.lastColor |= FOREGROUND_BLUE;
	}
	if ( color.a > 0 )
	{
		context.lastColor |= FOREGROUND_INTENSITY;
	}

	// Just use the initial color if there's a match
	if ( context.lastColor == context.badColor )
	{
		context.lastColor = context.initialColor;
	}

	SetConsoleTextAttribute( stdHandle, context.lastColor | context.backgroundColor );
	return prevColor;
}

/*
==================
Sys_RestoreConsoleColor
==================
*/
void Sys_RestoreConsoleColor( consoleColorContext_t& context, uint16 prevColor )
{
	HANDLE stdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	if ( GetFileType( stdHandle ) == FILE_TYPE_UNKNOWN )
	{
		return;
	}

	SetConsoleTextAttribute( stdHandle, prevColor | context.backgroundColor );
	context.lastColor = prevColor;
}
