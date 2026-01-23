#pragma once
#include "tier0/tier0.h"

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
	virtual void SetTextColor( logTextColor_t textColor )		  = 0;
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

	virtual void		   Printf( logGroup_t group, logLevel_t level, const char* pFormat, ... )			   = 0;
	virtual void		   VPrintf( logGroup_t group, logLevel_t level, const char* pFormat, va_list params ) = 0;
	virtual void		   SetTextColor( logTextColor_t textColor )											   = 0;
	virtual logTextColor_t GetTextColor() const																   = 0;
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

// NOTE: If the module was build with disabled logging the functions return a null implementation
TIER0_INTERFACE ILogger*			  Logger();
TIER0_INTERFACE ILogOutputConsole* LogConsoleOS();

#if ENABLE_LOGGING
// Functions for logging in 'general' group
TIER0_INTERFACE void Msg( const char* pFormat, ... );
TIER0_INTERFACE void Warning( const char* pFormat, ... );
TIER0_INTERFACE void Error( const char* pFormat, ... );

// Functions for logging in 'developer' group
TIER0_INTERFACE void DevMsg( const char* pFormat, ... );
TIER0_INTERFACE void DevWarning( const char* pFormat, ... );
TIER0_INTERFACE void DevError( const char* pFormat, ... );
#else
FORCEINLINE void Msg( const char* pFormat, ... ) {}
FORCEINLINE void Warning( const char* pFormat, ... ) {}
FORCEINLINE void Error( const char* pFormat, ... ) {}

FORCEINLINE void DevMsg( const char* pFormat, ... ) {}
FORCEINLINE void DevWarning( const char* pFormat, ... ) {}
FORCEINLINE void DevError( const char* pFormat, ... ) {}
#endif	// ENABLE_LOGGING

//-----------------------------------------------------------------------------
// Asserts
//-----------------------------------------------------------------------------
#if ENABLE_ASSERT
TIER0_INTERFACE bool Sys_AssertFailed( const char* pExpr, const char* pFile, int32 line, const char* pFormat = "", ... );

	#define Assert( Expr )				(void)( !!( Expr ) || Sys_AssertFailed( #Expr, __FILE__, __LINE__ ) )
	#define AssertMsg( Expr, Msg, ... ) (void)( !!( Expr ) || Sys_AssertFailed( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ) )
	#define AssertNoEntry()				Sys_AssertFailed( "Enclosing block should never be called", __FILE__, __LINE__ )
	#define AssertNoReentry()                                                                            \
		{                                                                                                \
			static bool CONCAT( s_bBeenHere_, __LINE__ ) = false;                                        \
			AssertMsg( !CONCAT( s_bBeenHere_, __LINE__ ), "Enclosing block was called more than once" ); \
			CONCAT( s_bBeenHere_, __LINE__ ) = true;                                                     \
		}
	#define AssertUnimplemented()		AssertMsg( false, "Unimplemented function called" )
	#define Verify( Expr )				( !!( Expr ) ? true : ( Sys_AssertFailed( #Expr, __FILE__, __LINE__ ), false ) )
	#define VerifyMsg( Expr, Msg, ... ) ( !!( Expr ) ? true : ( Sys_AssertFailed( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ), false ) )
#else
FORCEINLINE bool Sys_AssertFailed( const char* pExpr, const char* pFile, int32 line, const char* pFormat = "", ... ) { return false; }

	#define Assert( Expr )
	#define AssertMsg( Expr, Msg, ... )
	#define AssertNoEntry()
	#define AssertNoReentry()
	#define AssertUnimplemented()
	#define Verify( Expr )				!!( Expr )
	#define VerifyMsg( Expr, Msg, ... ) !!( Expr )
#endif	// ENABLE_ASSERT

//-----------------------------------------------------------------------------
// Ensures
//-----------------------------------------------------------------------------
#if ENABLE_ENSURE
TIER0_INTERFACE bool Sys_EnsureFailed( const char* pExpr, const char* pFile, int32 line, bool bAlways, const char* pFormat = "", ... );
TIER0_INTERFACE void Sys_SetEnsureAllow( bool bAllowed );

	#define Ensure( Expr )					  ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false ), false ) )
	#define EnsureMsg( Expr, Msg, ... )		  ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false, Msg, __VA_ARGS__ ), false ) )
	#define EnsureAlways( Expr )			  ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true ), false ) )
	#define EnsureAlwaysMsg( Expr, Msg, ... ) ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true, Msg, __VA_ARGS__ ), false ) )
#else
FORCEINLINE bool Sys_EnsureFailed( const char* pExpr, const char* pFile, int32 line, bool bAlways, const char* pFormat = "", ... ) { return false; }
FORCEINLINE void Sys_SetEnsureAllow( bool bAllowed ) {}

	#define Ensure( Expr )					  !!( Expr )
	#define EnsureMsg( Expr, Msg, ... )		  !!( Expr )
	#define EnsureAlways( Expr )			  !!( Expr )
	#define EnsureAlwaysMsg( Expr, Msg, ... ) !!( Expr )
#endif	// ENABLE_ENSURE
