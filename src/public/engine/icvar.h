#pragma once
#include "appframework/iappsystem.h"
#include "engine/iconcmd.h"
#include "engine/iconvar.h"
#include "stdlib/math/color.h"

//-----------------------------------------------------------------------------
// Applications can implement this to modify behavior in ICvar
//-----------------------------------------------------------------------------
#define CVAR_QUERY_INTERFACE_VERSION		"SCvarQuery001"
class ICvarQuery : public IAppSystem
{
public:
	virtual bool AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar ) = 0;
};


//-----------------------------------------------------------------------------
// Used to display console messages
//-----------------------------------------------------------------------------
class IConsoleDisplayFunc
{
public:
	virtual void Print( const achar* pMsg ) = 0;
	virtual void Print( const achar* pMsg, const CColor& color ) = 0;
};


//-----------------------------------------------------------------------------
// The console system interface
//-----------------------------------------------------------------------------
#define CVAR_INTERFACE_VERSION		"SCvar001"
class ICvar : public IAppSystem
{
public:
	virtual cvarDLLIdentifier_t AllocateDLLIdentifier() = 0;

	virtual void RegisterCommand( IConCmdBase* pCommand ) = 0;
	virtual void UnregisterCommand( IConCmdBase* pCommand ) = 0;
	virtual void UnregisterCommands( cvarDLLIdentifier_t dllIdentifier ) = 0;

	virtual bool Exec( const achar* pCommand ) = 0;

	virtual IConCmdBase* FindCommandBase( const achar* pName ) const = 0;
	virtual IConCmd* FindCommand( const achar* pName ) const = 0;
	virtual IConVar* FindVar( const achar* pName ) const = 0;

	// Sets cvars containing the flags to their default value
	virtual void ResetFlaggedVars( uint32 flags ) = 0;

	// Get the first IConCmdBase to allow iteration over all IConCmd and IConVars
	virtual IConCmdBase* GetCommands() const = 0;

	virtual void SetGlobalChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn ) = 0;
	virtual void CallGlobalChangeCallback( IConVar* pConVar ) = 0;

	virtual void SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc ) = 0;
	virtual void ConsolePrintf( const CColor& color, const achar* pFormat, ... ) = 0;
	virtual void ConsolePrintf( const achar* pFormat, ... ) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICvar, so we can't use the standard connect pattern
	// NOTE: pCvarQuery If NULL ICvar set to default ICvarQuery
	virtual void SetCVarQuery( ICvarQuery* pCvarQuery ) = 0;
};