#pragma once
#include "appframework/iappsystem.h"
#include "cvar/iconcmd.h"
#include "cvar/iconvar.h"
#include "tier1/delegate.h"
#include "tier1/math/color.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStreamDataWriter;

//-----------------------------------------------------------------------------
// Cvar delegate interfaces
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnWriteConCmdsToConfigFile, IStreamDataWriter* /* pStreamData */ );

//-----------------------------------------------------------------------------
// Applications can implement this to modify behavior in ICvar
//-----------------------------------------------------------------------------
#define CVAR_QUERY_INTERFACE_VERSION "SCvarQuery001"
class ICvarQuery : public IAppSystem
{
public:
	virtual bool AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar ) = 0;
};

//-----------------------------------------------------------------------------
// IConVars overrider
//-----------------------------------------------------------------------------
class IConVarsOverrider
{
public:
	virtual void OverrideFromCommandLine() = 0;
};

//-----------------------------------------------------------------------------
// Used to display console messages
//-----------------------------------------------------------------------------
class IConsoleDisplayFunc
{
public:
	virtual void Print( const char* pMsg )						= 0;
	virtual void Print( const char* pMsg, const CColor& color ) = 0;
};

//-----------------------------------------------------------------------------
// The console system interface
//-----------------------------------------------------------------------------
#define CVAR_INTERFACE_VERSION "SCvar001"
class ICvar : public IAppSystem
{
public:
	virtual cvarDLLIdentifier_t AllocateDLLIdentifier() = 0;

	virtual void RegisterCommand( IConCmdBase* pCommand )				 = 0;
	virtual void UnregisterCommand( IConCmdBase* pCommand )				 = 0;
	virtual void UnregisterCommands( cvarDLLIdentifier_t dllIdentifier ) = 0;

	virtual bool Exec( const char* pCommand ) = 0;

	virtual IConCmdBase* FindCommandBase( const char* pName, uint32 length ) const = 0;
	virtual IConCmdBase* FindCommandBase( const char* pName ) const				   = 0;
	virtual IConCmd*	 FindCommand( const char* pName, uint32 length ) const	   = 0;
	virtual IConCmd*	 FindCommand( const char* pName ) const					   = 0;
	virtual IConVar*	 FindVar( const char* pName, uint32 length ) const		   = 0;
	virtual IConVar*	 FindVar( const char* pName ) const						   = 0;

	// Read and write a configuration file
	virtual void						 ReadConfigFile( const char* pConfigDir, bool bWriteConfigIfNotExist = true ) = 0;
	virtual void						 WriteConfigFile( const char* pConfigDir, bool bWriteDefaultConfig = false )  = 0;
	virtual IOnWriteConCmdsToConfigFile* OnWriteConCmdsToConfigFile() const											  = 0;

	// Override IConVars from a command line
	virtual void OverrideConVarsFromCommandLine()																= 0;
	virtual void SetConVarsOverrider( cvarDLLIdentifier_t dllIdentifier, IConVarsOverrider* pConVarsOverrider ) = 0;

	// Sets cvars containing the flags to their default value
	virtual void ResetFlaggedVars( uint32 flags ) = 0;

	// Get the first IConCmdBase to allow iteration over all IConCmd and IConVars
	virtual IConCmdBase* GetCommands() const = 0;

	virtual void SetGlobalChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn ) = 0;
	virtual void CallGlobalChangeCallback( IConVar* pConVar )						   = 0;

	virtual void SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc ) = 0;
	virtual void ConsolePrintf( const CColor& color, const char* pFormat, ... )	   = 0;
	virtual void ConsolePrintf( const char* pFormat, ... )						   = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICvar, so we can't use the standard connect pattern
	// NOTE: pCvarQuery If NULL ICvar set to default ICvarQuery
	virtual void SetCVarQuery( ICvarQuery* pCvarQuery ) = 0;
};
