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

CORE_INTERFACE void Sys_SetLogOutputFunc( logOutputFn_t pFunc );
CORE_INTERFACE logOutputFn_t Sys_GetLogOutputFunc();
CORE_INTERFACE logOutputFn_t Sys_GetDefaultLogOutput();
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
    CORE_INTERFACE void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... );

    #define AssertCode( Code ) \
        do \
        { \
            Code \
        } \
        while ( false );

    #define AssertMsg( Expr, Msg, ... )	\
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ); \
        } \
    }

    #define AssertFunc( Expr, Func ) \
    { \
        if ( !( Expr ) ) \
        { \
            Func; \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__ ); \
        } \
    }

    #define Assert( Expr ) \
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__ ); \
        } \
    }

    #define AssertNoEntry()                     Sys_FailAssertFunc( "Enclosing block should never be called", __FILE__, __LINE__ );
    #define AssertNoReentry() \
    { \
        static bool s_beenHere##__LINE__ = false; \
        AssertMsg( !s_beenHere##__LINE__, "Enclosing block was called more than once" ); \
        s_beenHere##__LINE__ = true; \
    }
#else
    FORCEINLINE void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... ) {}
    #define AssertCode( Code )		        {}
    #define AssertMsg( Expr, Msg, ... )		{}
    #define AssertFunc( Expr, Func )	    {}
    #define Assert( Expr )				    {}
    #define AssertNoEntry()                 {}
    #define AssertNoReentry()               {}
#endif // ENABLE_ASSERT

#endif // !DEBUG_H