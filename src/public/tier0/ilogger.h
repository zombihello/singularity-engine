#pragma once
#include "tier0/debug.h"
#include "tier0/consoleio.h"
#include "tier1/strtools.h"

//-----------------------------------------------------------------------------
// Constants and types
//-----------------------------------------------------------------------------
enum
{
	// An invalid color set on a log output to imply that it should use
	// a device-dependent default color where applicable
	UNSPECIFIED_LOG_COLOR = 0
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

struct logContext_t
{
	logGroup_t group;
	logLevel_t level;
	CColor	   color;
};

//-----------------------------------------------------------------------------
// Interface for classes to handle logging output
//-----------------------------------------------------------------------------
class ILogOutput
{
public:
	virtual void Print( const logContext_t& context, const char* pMessage ) = 0;
};

//-----------------------------------------------------------------------------
// The central logging system interface
//-----------------------------------------------------------------------------
class ILogger
{
public:
	virtual void AddOutput( ILogOutput* pLogOutput )	= 0;
	virtual void RemoveOutput( ILogOutput* pLogOutput ) = 0;

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) = 0;
	virtual bool IsGroupActive( logGroup_t group ) const			  = 0;

	virtual void Printf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, ... )			 = 0;
	virtual void VPrintf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, va_list params ) = 0;
};

//-----------------------------------------------------------------------------
// A log output into stdout
//-----------------------------------------------------------------------------
class CLogOutputStdOut : public ILogOutput
{
public:
	// ILogOutput interface
	virtual void Print( const logContext_t& context, const char* pMessage ) override;

	CLogOutputStdOut();

private:
	consoleColorContext_t colorContext;
};

//-----------------------------------------------------------------------------
// Functions for logging in 'general' group
//-----------------------------------------------------------------------------
void Msg( const char* pFormat, ... );
void Msg( const CColor& color, const char* pFormat, ... );
void Warning( const char* pFormat, ... );
void Warning( const CColor& color, const char* pFormat, ... );
void Error( const char* pFormat, ... );
void Error( const CColor& color, const char* pFormat, ... );

//-----------------------------------------------------------------------------
// Functions for logging in 'developer' group
//-----------------------------------------------------------------------------
void DevMsg( const char* pFormat, ... );
void DevMsg( const CColor& color, const char* pFormat, ... );
void DevWarning( const char* pFormat, ... );
void DevWarning( const CColor& color, const char* pFormat, ... );
void DevError( const char* pFormat, ... );
void DevError( const CColor& color, const char* pFormat, ... );

// NOTE: If the module was build with disabled logging the functions return a null implementation
TIER0_INTERFACE ILogger* Logger();

#include "tier0/ilogger.inl"
