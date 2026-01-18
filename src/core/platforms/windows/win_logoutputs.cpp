#include "pch_core.h"
#include "core/logoutput_console_null.h"

#if ENABLE_LOGGING
static const uint16 s_LogTextColorsWin32[] = {
	0x7,  // LOG_TEXT_COLOR_DEFAULT
	0x7,  // LOG_TEXT_COLOR_WHITE
	0xC,  // LOG_TEXT_COLOR_RED
	0xE,  // LOG_TEXT_COLOR_YELLOW
	0x2	  // LOG_TEXT_COLOR_GREEN
};
static_assert( ARRAYSIZE( s_LogTextColorsWin32 ) == LOG_TEXT_NUM_COLORS, "Array size 's_LogTextColorsWin32' must be equal to LOG_TEXT_NUM_COLORS" );

// A Windows log output to debug (i.g: Output window in Visual Studio).
class CWindowsLogOutputDebug : public CBaseLogOutput<ILogOutput>
{
public:
	// ILogOutput interface
	virtual void Print( logLevel_t level, const char* pMessage ) override;
};

// A Windows log output to a console
class CWindowsLogOutputConsole : public CBaseLogOutput<ILogOutputConsole>
{
public:
	// ILogOutput interface
	virtual void SetTextColor( logTextColor_t textColor ) override;
	virtual void Print( logLevel_t level, const char* pMessage ) override;

	// ILogOutputConsole interface
	virtual void Show( bool bShowConsole ) override;
	virtual bool IsShown() const override;

	CWindowsLogOutputConsole();
	~CWindowsLogOutputConsole();

private:
	HANDLE consoleHandle;
};

/*
==================
CWindowsLogOutputDebug::Print
==================
*/
void CWindowsLogOutputDebug::Print( logLevel_t level, const char* pMessage )
{
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugMessage( pMessage );
	}
}

/*
==================
CWindowsLogOutputConsole::CWindowsLogOutputConsole
==================
*/
CWindowsLogOutputConsole::CWindowsLogOutputConsole()
	: consoleHandle( NULL )
{
}

/*
==================
CWindowsLogOutputConsole::~CWindowsLogOutputConsole
==================
*/
CWindowsLogOutputConsole::~CWindowsLogOutputConsole()
{
	Show( false );
}

/*
==================
CWindowsLogOutputConsole::Show
==================
*/
void CWindowsLogOutputConsole::Show( bool bShowConsole )
{
	if ( bShowConsole && !consoleHandle )
	{
		AllocConsole();
		consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		freopen( "conin$", "r", stdin );
		freopen( "conout$", "w", stdout );
		freopen( "conout$", "w", stderr );
		SetTextColor( textColor );
	}
	else if ( !bShowConsole && consoleHandle )
	{
		consoleHandle = NULL;
		FreeConsole();
	}
}

/*
==================
CWindowsLogOutputConsole::IsShown
==================
*/
bool CWindowsLogOutputConsole::IsShown() const
{
	return !!consoleHandle;
}

/*
==================
CWindowsLogOutputConsole::SetTextColor
==================
*/
void CWindowsLogOutputConsole::SetTextColor( logTextColor_t textColor )
{
	if ( consoleHandle && CWindowsLogOutputConsole::textColor != textColor )
	{
		SetConsoleTextAttribute( consoleHandle, s_LogTextColorsWin32[(uint32)textColor] );
	}
	CBaseLogOutput<ILogOutputConsole>::SetTextColor( textColor );
}

/*
==================
CWindowsLogOutputConsole::Print
==================
*/
void CWindowsLogOutputConsole::Print( logLevel_t level, const char* pMessage )
{
	if ( consoleHandle )
	{
		logTextColor_t originalTextColor = textColor;
		bool		   bNeedResetColor	 = true;
		switch ( level )
		{
		case LOG_LEVEL_ERROR: SetTextColor( LOG_TEXT_COLOR_RED ); break;
		case LOG_LEVEL_WARNING: SetTextColor( LOG_TEXT_COLOR_YELLOW ); break;
		default: bNeedResetColor = false; break;
		}

		printf( pMessage );
		if ( bNeedResetColor )
		{
			SetTextColor( originalTextColor );
		}
	}
}

/*
==================
Sys_SetupDefaultLogOutputs
==================
*/
void Sys_SetupDefaultLogOutputs( ILogger* pLogger )
{
	pLogger->AddOutput( new CWindowsLogOutputDebug() );
	pLogger->AddOutput( LogConsoleOS() );
}
#endif	// ENABLE_LOGGING

/*
==================
LogConsoleOS
==================
*/
ILogOutputConsole* LogConsoleOS()
{
#if ENABLE_LOGGING
	static CWindowsLogOutputConsole s_LogOutputConsole;
	return &s_LogOutputConsole;
#else
	static CNullLogOutputConsole s_NullLogOutputConsole;
	return &s_NullLogOutputConsole;
#endif	// ENABLE_LOGGING
}
