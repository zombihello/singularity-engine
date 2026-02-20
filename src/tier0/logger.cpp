#include "pch_tier0.h"
#include "tier0/ilogger.h"

#if ENABLE_LOGGING
class CLogger : public ILogger
{
public:
	// ILogger interface
	virtual void AddOutput( ILogOutput* pLogOutput ) override;
	virtual void RemoveOutput( ILogOutput* pLogOutput ) override;
	virtual void RemoveAllOutputs() override;

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) override;
	virtual bool IsGroupActive( logGroup_t group ) const override;

	virtual void		   Printf( logGroup_t group, logLevel_t level, const char* pFormat, ... ) override;
	virtual void		   VPrintf( logGroup_t group, logLevel_t level, const char* pFormat, va_list params ) override;
	virtual void		   SetTextColor( logTextColor_t textColor ) override;
	virtual logTextColor_t GetTextColor() const override;

	CLogger();

private:
	eastl::vector<ILogOutput*> outputs;
	bool					   bActiveGroups[LOG_NUM_GROUPS];
	logTextColor_t			   textColor;
};

static const char* s_pLogLevelNames[] = {
	"Msg",		// LOG_LEVEL_MESSAGE
	"Warning",	// LOG_LEVEL_WARNING
	"Error"		// LOG_LEVEL_ERROR
};
static_assert( ARRAYSIZE( s_pLogLevelNames ) == LOG_NUM_LEVELS, "Invalid array size of s_pLogLevelNames, must be equal to LOG_NUM_LEVELS" );

/*
==================
CLogger::CLogger
==================
*/
CLogger::CLogger()
	: textColor( LOG_TEXT_COLOR_DEFAULT )
{
	bActiveGroups[LOG_GROUP_GENERAL]   = true;
	bActiveGroups[LOG_GROUP_DEVELOPER] = false;
	Sys_SetupDefaultLogOutputs( this );
}

/*
==================
CLogger::AddOutput
==================
*/
void CLogger::AddOutput( ILogOutput* pLogOutput )
{
	outputs.emplace_back( pLogOutput );
	pLogOutput->SetTextColor( textColor );
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
CLogger::RemoveAllOutputs
==================
*/
void CLogger::RemoveAllOutputs()
{
	outputs.clear();
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
void CLogger::Printf( logGroup_t group, logLevel_t level, const char* pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	VPrintf( group, level, pFormat, params );
	va_end( params );
}

/*
==================
CLogger::VPrintf
==================
*/
void CLogger::VPrintf( logGroup_t group, logLevel_t level, const char* pFormat, va_list params )
{
	if ( IsGroupActive( group ) )
	{
		Assert( level < LOG_NUM_LEVELS );
		eastl::string message = S_Sprintf( "%s: %s\n", s_pLogLevelNames[level], S_Vsprintf( pFormat, params ).c_str() );
		for ( uint32 index = 0, count = (uint32)outputs.size(); index < count; ++index )
		{
			outputs[index]->Print( level, message.c_str() );
		}
	}
}

/*
==================
CLogger::SetTextColor
==================
*/
void CLogger::SetTextColor( logTextColor_t textColor )
{
	CLogger::textColor = textColor;
	for ( uint32 index = 0, count = (uint32)outputs.size(); index < count; ++index )
	{
		outputs[index]->SetTextColor( textColor );
	}
}

/*
==================
CLogger::GetTextColor
==================
*/
logTextColor_t CLogger::GetTextColor() const
{
	return textColor;
}
#else
class CNullLogger : public ILogger
{
public:
	// ILogger interface
	virtual void AddOutput( ILogOutput* pLogOutput ) {}
	virtual void RemoveOutput( ILogOutput* pLogOutput ) {}
	virtual void RemoveAllOutputs() {}

	virtual void SetGroupActivate( logGroup_t group, bool bActivate ) {}
	virtual bool IsGroupActive( logGroup_t group ) const { return false; }

	virtual void		   Printf( logGroup_t group, logLevel_t level, const char* pFormat, ... ) {}
	virtual void		   VPrintf( logGroup_t group, logLevel_t level, const char* pFormat, va_list params ) {}
	virtual void		   SetTextColor( logTextColor_t textColor ) {}
	virtual logTextColor_t GetTextColor() const { return LOG_TEXT_COLOR_DEFAULT; }
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
