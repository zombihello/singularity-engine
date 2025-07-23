/**
 * @file
 * @addtogroup core core
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "core/core.h"

//-----------------------------------------------------------------------------
// Debug log
//-----------------------------------------------------------------------------
typedef void ( *logOutputFn_t )( const achar* pMsg );
enum logColor_t
{
    LOG_COLOR_DEFAULT,
    LOG_COLOR_WHITE,
    LOG_COLOR_RED,
    LOG_COLOR_YELLOW,
    LOG_COLOR_GREEN,
    LOG_NUM_COLORS
};


CORE_INTERFACE void Sys_SetLogOutputFunc( logOutputFn_t pFunc );
CORE_INTERFACE logOutputFn_t Sys_GetLogOutputFunc();
CORE_INTERFACE logOutputFn_t Sys_GetDefaultLogOutput();
CORE_INTERFACE void Sys_SetLogColor( logColor_t logColor );
CORE_INTERFACE void Sys_ResetLogColor();
CORE_INTERFACE void Sys_SetupConsoleIO();
CORE_INTERFACE bool Sys_IsInitedConsoleIO();

#if ENABLE_LOGGING
    CORE_INTERFACE void Msg( const achar* pFormat, ... );
    CORE_INTERFACE void VMsg( const achar* pFormat, va_list params );
    CORE_INTERFACE void Warning( const achar* pFormat, ... );
    CORE_INTERFACE void VWarning( const achar* pFormat, va_list params );
    CORE_INTERFACE void Error( const achar* pFormat, ... );
    CORE_INTERFACE void VError( const achar* pFormat, va_list params );
#else
    FORCEINLINE void Msg( const achar* pFormat, ... )                   {}
    FORCEINLINE void VMsg( const achar* pFormat, va_list params )       {}
    FORCEINLINE void Warning( const achar* pFormat, ... )               {}
    FORCEINLINE void VWarning( const achar* pFormat, va_list params )   {}
    FORCEINLINE void Error( const achar* pFormat, ... )                 {}
    FORCEINLINE void VError( const achar* pFormat, va_list params )     {}
#endif // ENABLE_LOGGING


//-----------------------------------------------------------------------------
// Asserts
//-----------------------------------------------------------------------------
#if ENABLE_ASSERT
    CORE_INTERFACE bool Sys_AssertFailed( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... );

    #define Assert( Expr )					    ( void )( !!( Expr ) || Sys_AssertFailed( #Expr, __FILE__, __LINE__ ) )
    #define AssertMsg( Expr, Msg, ... )		    ( void )( !!( Expr ) || Sys_AssertFailed( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ) )
    #define AssertNoEntry()					    Sys_AssertFailed( "Enclosing block should never be called", __FILE__, __LINE__ )
    #define AssertNoReentry() \
    { \
        static bool CONCAT( s_bBeenHere_, __LINE__ ) = false; \
        AssertMsg( !CONCAT( s_bBeenHere_, __LINE__ ), "Enclosing block was called more than once" ); \
        CONCAT( s_bBeenHere_, __LINE__ ) = true; \
    }
    #define AssertUnimplemented()               AssertMsg( false, "Unimplemented function called" )
    #define Verify( Expr )                      ( !!( Expr ) ? true : ( Sys_AssertFailed( #Expr, __FILE__, __LINE__ ), false ) )
    #define VerifyMsg( Expr, Msg, ... )         ( !!( Expr ) ? true : ( Sys_AssertFailed( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ), false ) )
#else
    FORCEINLINE bool Sys_AssertFailed( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... ) { return false; }

    #define Assert( Expr )
    #define AssertMsg( Expr, Msg, ... )
    #define AssertNoEntry()
    #define AssertNoReentry()
    #define AssertUnimplemented()
    #define Verify( Expr )                      !!( Expr )
    #define VerifyMsg( Expr, Msg, ... )         !!( Expr )
#endif // ENABLE_ASSERT


//-----------------------------------------------------------------------------
// Ensures
//-----------------------------------------------------------------------------
#if ENABLE_ENSURE
    CORE_INTERFACE bool Sys_EnsureFailed( const achar* pExpr, const achar* pFile, int32 line, bool bAlways, const achar* pFormat = "", ... );
    CORE_INTERFACE void Sys_SetEnsureAllow( bool bAllowed );

    #define Ensure( Expr )                        ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false ), false ) )
    #define EnsureMsg( Expr, Msg, ... )           ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false, Msg, __VA_ARGS__ ), false ) )
    #define EnsureAlways( Expr )                  ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true ), false ) )
    #define EnsureAlwaysMsg( Expr, Msg, ... )     ( ( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true, Msg, __VA_ARGS__ ), false ) )
#else
    FORCEINLINE bool Sys_EnsureFailed( const achar* pExpr, const achar* pFile, int32 line, bool bAlways, const achar* pFormat = "", ... ) { return false; }
    FORCEINLINE void Sys_SetEnsureAllow( bool bAllowed ) {}

    #define Ensure( Expr )                        !!( Expr )
    #define EnsureMsg( Expr, Msg, ... )           !!( Expr )
    #define EnsureAlways( Expr )                  !!( Expr )
    #define EnsureAlwaysMsg( Expr, Msg, ... )     !!( Expr )
#endif // ENABLE_ENSURE

#endif // !DEBUG_H