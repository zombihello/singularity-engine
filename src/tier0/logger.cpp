#include "pch_tier0.h"
#include "tier0/debug.h"
#include "tier0/ilogger.h"

#if ENABLE_LOGGING
class CLogOutputDebug : public ILogOutput
{
public:
	virtual void Print( const logContext_t& context, const char* pMessage ) override;
};

class CLogger : public ILogger
{
public:
	// ILogger interface
	virtual void AddOutput( ILogOutput* pLogOutput ) override;
	virtual void RemoveOutput( ILogOutput* pLogOutput ) override;

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) override;
	virtual bool IsGroupActive( logGroup_t group ) const override;

	virtual void Printf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, ... ) override;
	virtual void VPrintf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, va_list params ) override;

	CLogger();

private:
	bool					   bActiveGroups[LOG_NUM_GROUPS];
	CLogOutputDebug			   defaultOutput;
	eastl::vector<ILogOutput*> outputs;
};

static const char* s_pLogLevelNames[] = {
	"Msg",		// LOG_LEVEL_MESSAGE
	"Warning",	// LOG_LEVEL_WARNING
	"Error"		// LOG_LEVEL_ERROR
};

static const CColor s_logLevelColors[] = {
	UNSPECIFIED_LOG_COLOR,		 // LOG_LEVEL_MESSAGE
	CColor( 255, 255, 0, 255 ),	 // LOG_LEVEL_WARNING
	CColor( 255, 0, 0, 255 )	 // LOG_LEVEL_ERROR
};

static_assert( ARRAYSIZE( s_pLogLevelNames ) == LOG_NUM_LEVELS, "Invalid array size of s_pLogLevelNames, must be equal to LOG_NUM_LEVELS" );
static_assert( ARRAYSIZE( s_logLevelColors ) == LOG_NUM_LEVELS, "Invalid array size of s_logLevelColors, must be equal to LOG_NUM_LEVELS" );

/*
==================
CLogOutputDebug::Print
==================
*/
void CLogOutputDebug::Print( const logContext_t& context, const char* pMessage )
{
	if ( Sys_IsDebuggerAttached() )
	{
		Sys_DebugMessage( pMessage );
	}
}

/*
==================
CLogger::CLogger
==================
*/
CLogger::CLogger()
{
	bActiveGroups[LOG_GROUP_GENERAL]   = true;
	bActiveGroups[LOG_GROUP_DEVELOPER] = false;
	AddOutput( &defaultOutput );
}

/*
==================
CLogger::AddOutput
==================
*/
void CLogger::AddOutput( ILogOutput* pLogOutput )
{
	outputs.emplace_back( pLogOutput );
}

/*
==================
CLogger::RemoveOutput
==================
*/
void CLogger::RemoveOutput( ILogOutput* pLogOutput )
{
	for ( uint32 index = 0, count = (uint32)outputs.size(); index < count; ++index )
	{
		if ( outputs[index] == pLogOutput )
		{
			outputs.erase( outputs.begin() + index );
			return;
		}
	}
}

/*
==================
CLogger::SetGroupActivate
==================
*/
void CLogger::SetGroupActivate( logGroup_t group, bool bActivate )
{
	Assert( group < LOG_NUM_GROUPS );
	bActiveGroups[group] = bActivate;
}

/*
==================
CLogger::IsGroupActive
==================
*/
bool CLogger::IsGroupActive( logGroup_t group ) const
{
	Assert( group < LOG_NUM_GROUPS );
	return bActiveGroups[group];
}

/*
==================
CLogger::Printf
==================
*/
void CLogger::Printf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	VPrintf( group, level, color, pFormat, params );
	va_end( params );
}

/*
==================
CLogger::VPrintf
==================
*/
void CLogger::VPrintf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, va_list params )
{
	if ( IsGroupActive( group ) )
	{
		Assert( level < LOG_NUM_LEVELS );
		eastl::string message = S_Sprintf( "%s: %s\n", s_pLogLevelNames[level], S_Vsprintf( pFormat, params ).c_str() );
		logContext_t  context;
		context.group = group;
		context.level = level;
		context.color = color != UNSPECIFIED_LOG_COLOR ? color : s_logLevelColors[level];
		for ( uint32 index = 0, count = (uint32)outputs.size(); index < count; ++index )
		{
			outputs[index]->Print( context, message.c_str() );
		}
	}
}
#else
class CNullLogger : public ILogger
{
public:
	// ILogger interface
	virtual void AddOutput( ILogOutput* pLogOutput ) override {}
	virtual void RemoveOutput( ILogOutput* pLogOutput ) override {}

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) override {}
	virtual bool IsGroupActive( logGroup_t group ) const override { return false; }

	virtual void Printf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, ... ) override {}
	virtual void VPrintf( logGroup_t group, logLevel_t level, const CColor& color, const char* pFormat, va_list params ) override {}
};
#endif	// ENABLE_LOGGING

/*
==================
Logger
==================
*/
ILogger* Logger()
{
#if ENABLE_LOGGING
	static CLogger s_Logger;
	return &s_Logger;
#else
	static CNullLogger s_NullLogger;
	return &s_NullLogger;
#endif	// ENABLE_LOGGING
}
