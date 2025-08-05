#include "pch_core.h"
#include "core/crashdump_private.h"

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

	virtual void Printf( logGroup_t group, logLevel_t level, const achar* pFormat, ... ) override;
	virtual void VPrintf( logGroup_t group, logLevel_t level, const achar* pFormat, va_list params ) override;
	virtual void SetTextColor( logTextColor_t textColor ) override;
	virtual logTextColor_t GetTextColor() const override;

	CLogger();

private:
	std::vector<ILogOutput*>	outputs;
	bool						bActiveGroups[LOG_NUM_GROUPS];
	logTextColor_t				textColor;
};

static const achar*				s_pLogLevelNames[] =
{
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
	bActiveGroups[LOG_GROUP_GENERAL]	= true;
	bActiveGroups[LOG_GROUP_DEVELOPER]	= false;
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
	for ( uint32 index = 0, count = ( uint32 )outputs.size(); index < count; ++index )
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
void CLogger::Printf( logGroup_t group, logLevel_t level, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	VPrintf( group, level, pFormat, params );
	va_end( params );
}

/*
==================
CLogger::VPrintf
==================
*/
void CLogger::VPrintf( logGroup_t group, logLevel_t level, const achar* pFormat, va_list params )
{
	if ( IsGroupActive( group ) )
	{
		Assert( level < LOG_NUM_LEVELS );
		std::string		message = S_Sprintf( "%s: %s\n", s_pLogLevelNames[level], S_Vsprintf( pFormat, params ).c_str() );
		for ( uint32 index = 0, count = ( uint32 )outputs.size(); index < count; ++index )
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
	for ( uint32 index = 0, count = ( uint32 )outputs.size(); index < count; ++index )
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


/*
==================
Msg
==================
*/
void Msg( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	Logger()->VPrintf(LOG_GROUP_GENERAL, LOG_LEVEL_MESSAGE, pFormat, params);
	va_end( params );
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
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_WARNING, pFormat, params );
	va_end( params );
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
	Logger()->VPrintf( LOG_GROUP_GENERAL, LOG_LEVEL_ERROR, pFormat, params );
	va_end( params );
}

/*
==================
DevMsg
==================
*/
void DevMsg( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_MESSAGE, pFormat, params );
	va_end( params );
}

/*
==================
DevWarning
==================
*/
void DevWarning( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_WARNING, pFormat, params );
	va_end( params );
}

/*
==================
DevError
==================
*/
void DevError( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	Logger()->VPrintf( LOG_GROUP_DEVELOPER, LOG_LEVEL_ERROR, pFormat, params );
	va_end( params );
}
#else
class CNullLogger : public ILogger
{
public:
	// ILogger interface
	virtual void AddOutput( ILogOutput* pLogOutput )														{}
	virtual void RemoveOutput( ILogOutput* pLogOutput )														{}
	virtual void RemoveAllOutputs()																			{}

	virtual void SetGroupActivate( logGroup_t group, bool bActivate )										{}
	virtual bool IsGroupActive( logGroup_t group ) const													{ return false; }

	virtual void Printf( logGroup_t group, logLevel_t level, const achar* pFormat, ... )					{}
	virtual void VPrintf( logGroup_t group, logLevel_t level, const achar* pFormat, va_list params )		{}
	virtual void SetTextColor( logTextColor_t textColor )													{}
	virtual logTextColor_t GetTextColor() const																{ return LOG_TEXT_COLOR_DEFAULT; }
};
#endif // ENABLE_LOGGING

/*
==================
Logger
==================
*/
ILogger* Logger()
{
#if ENABLE_LOGGING
	static CLogger			s_Logger;
	return &s_Logger;
#else
	static CNullLogger		s_NullLogger;
	return &s_NullLogger;
#endif // ENABLE_LOGGING
}


#if ENABLE_ASSERT
/*
==================
Sys_AssertFailed
==================
*/
bool Sys_AssertFailed( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat /*= "" */, ... )
{
	// Don't show message if we already shutdown the game by a critical error
	static bool		s_bAlreadyHasError = false;
	if ( s_bAlreadyHasError )
	{
		return false;
	}
	s_bAlreadyHasError = true;

	// Get the message
	va_list			params;
	va_start( params, pFormat );
	std::string		message = S_Strlen( pFormat ) > 0 ? S_Sprintf( pFormat, params ) : "<None>";
	va_end( params );

	// Print the message and show message box
	Error( "------------ ASSERTION FAILED --------------" );
	Error( "Expression: %s", pExpr );
	Error( "Message: %s", message.c_str() );
	Error( "" );
	Error( "File: %s", pFile );
	Error( "Line: %i", line );
	Error( "--------------------------------------------" );

	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugBreak();
	}
	std::string		fullMessage = S_Sprintf( "Expression: %s\nMessage: %s\n\nFile: %s\nLine: %i", pExpr, message.c_str(), pFile, line );
	Sys_ShowMessageBox( "Singularity Error", fullMessage.c_str(), MESSAGE_BOX_ERROR );

	// Set crash dump message
	CrashDump_SetMessage( fullMessage.c_str() );

	// Shutdown application
	Sys_RequestExit( true );
	return true;
}
#endif // ENABLE_ASSERT


#if ENABLE_ENSURE
static bool		s_bEnsureAllowed = true;

/*
==================
Sys_EnsureFailed
==================
*/
bool Sys_EnsureFailed( const achar* pExpr, const achar* pFile, int32 line, bool bAlways, const achar* pFormat /*= ""*/, ... )
{
	if ( bAlways || s_bEnsureAllowed )
	{
		// Get the final message
		va_list			params;
		va_start( params, pFormat );
		std::string		message = S_Strlen( pFormat ) > 0 ? S_Sprintf( pFormat, params ) : "<None>";
		va_end( params );

		// Print the message
		Error( "------------ ENSURE FAILED --------------" );
		Error( "Expression: %s", pExpr );
		Error( "Message: %s", message.c_str() );
		Error( "" );
		Error( "File: %s", pFile );
		Error( "Line: %i", line );
		Error( "--------------------------------------------" );

		if ( Sys_IsDebuggerPresent() )
		{
			Sys_DebugBreak();
		}
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
#endif // ENABLE_ENSURE