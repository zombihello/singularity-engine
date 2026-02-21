#pragma once
#include "tier0/tier0_api.h"
#include "tier0/defines.h"
#include "tier0/types.h"

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
	#define Verify( Expr )				( Expr )
	#define VerifyMsg( Expr, Msg, ... ) ( Expr )
#endif	// ENABLE_ASSERT

//-----------------------------------------------------------------------------
// Ensures
//-----------------------------------------------------------------------------
#if ENABLE_ENSURE
TIER0_INTERFACE bool Sys_EnsureFailed( const char* pExpr, const char* pFile, int32 line, bool bAlways, const char* pFormat = "", ... );
TIER0_INTERFACE void Sys_SetEnsureAllow( bool bAllowed );

	#define Ensure( Expr )					  ( !!( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false ), false ) )
	#define EnsureMsg( Expr, Msg, ... )		  ( !!( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, false, Msg, __VA_ARGS__ ), false ) )
	#define EnsureAlways( Expr )			  ( !!( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true ), false ) )
	#define EnsureAlwaysMsg( Expr, Msg, ... ) ( !!( Expr ) ? true : ( Sys_EnsureFailed( #Expr, __FILE__, __LINE__, true, Msg, __VA_ARGS__ ), false ) )
#else
FORCEINLINE bool Sys_EnsureFailed( const char* pExpr, const char* pFile, int32 line, bool bAlways, const char* pFormat = "", ... ) { return false; }
FORCEINLINE void Sys_SetEnsureAllow( bool bAllowed ) {}

	#define Ensure( Expr )					  ( Expr )
	#define EnsureMsg( Expr, Msg, ... )		  ( Expr )
	#define EnsureAlways( Expr )			  ( Expr )
	#define EnsureAlwaysMsg( Expr, Msg, ... ) ( Expr )
#endif	// ENABLE_ENSURE
