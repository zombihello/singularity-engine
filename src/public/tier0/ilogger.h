#pragma once
#include "tier0/tier0_api.h"
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Logger
//-----------------------------------------------------------------------------
enum logTextColor_t
{
	LOG_TEXT_COLOR_DEFAULT,
	LOG_TEXT_COLOR_WHITE,
	LOG_TEXT_COLOR_RED,
	LOG_TEXT_COLOR_YELLOW,
	LOG_TEXT_COLOR_GREEN,
	LOG_TEXT_NUM_COLORS
};

enum logGroup_t
{
	LOG_GROUP_GENERAL,
	LOG_GROUP_DEVELOPER,
	LOG_NUM_GROUPS
};

enum logLevel_t
{
	LOG_LEVEL_MESSAGE,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_NUM_LEVELS
};

// A log output
class ILogOutput
{
public:
	virtual void SetTextColor( logTextColor_t textColor )		 = 0;
	virtual void Print( logLevel_t level, const char* pMessage ) = 0;
};

// A log output into a console
class ILogOutputConsole : public ILogOutput
{
public:
	virtual void Show( bool bShowConsole ) = 0;
	virtual bool IsShown() const		   = 0;
};

// The logger system that supports multiple outputs
// (i.g: output into a console, file, etc)
class ILogger
{
public:
	virtual void AddOutput( ILogOutput* pLogOutput )	= 0;
	virtual void RemoveOutput( ILogOutput* pLogOutput ) = 0;
	virtual void RemoveAllOutputs()						= 0;

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) = 0;
	virtual bool IsGroupActive( logGroup_t group ) const			  = 0;

	virtual void		   Printf( logGroup_t group, logLevel_t level, const char* pFormat, ... )			  = 0;
	virtual void		   VPrintf( logGroup_t group, logLevel_t level, const char* pFormat, va_list params ) = 0;
	virtual void		   SetTextColor( logTextColor_t textColor )											  = 0;
	virtual logTextColor_t GetTextColor() const																  = 0;
};

// Helper base implementation of an ILogOutput
template<class TBaseClass>
class CBaseLogOutput : public TBaseClass
{
public:
	// ILogOutput interface
	virtual void SetTextColor( logTextColor_t textColor ) override { CBaseLogOutput::textColor = textColor; }

	CBaseLogOutput()
		: textColor( LOG_TEXT_COLOR_DEFAULT )
	{
	}

protected:
	logTextColor_t textColor;
};

// Functions for logging in 'general' group
void Msg( const char* pFormat, ... );
void Warning( const char* pFormat, ... );
void Error( const char* pFormat, ... );

// Functions for logging in 'developer' group
void DevMsg( const char* pFormat, ... );
void DevWarning( const char* pFormat, ... );
void DevError( const char* pFormat, ... );

// NOTE: If the module was build with disabled logging the functions return a null implementation
TIER0_INTERFACE ILogger*		   Logger();
TIER0_INTERFACE ILogOutputConsole* LogConsoleOS();

#include "tier0/ilogger.inl"
