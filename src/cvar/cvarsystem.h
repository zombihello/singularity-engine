#pragma once
#include "cvar/icvarsystem.h"

//-----------------------------------------------------------------------------
// The console variable system
//-----------------------------------------------------------------------------
class CCVarSystem : public CBaseAppSystem<ICVarSystem>
{
public:
	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// ICVarSystem interface
	// Allocate a new identifier for a DLL
	virtual cvarDLLIdentifier_t AllocateDLLIdentifier() override;

	// Register and unregister a cvar
	// NOTE: If set dllIdentifier then will be removed a cvar of specific a dll module
	virtual void RegisterVariable( ICVar* pCVar ) override;
	virtual void UnregisterVariable( ICVar* pCVar ) override;
	virtual void UnregisterAllVariables( cmdDLLIdentifier_t dllIdentifier = INVALID_INDEX ) override;

	// Finds the cvar with the given name
	// Returns NULL if there is no cvar with the given name
	virtual ICVar* FindVariable( const char* pName, uint32 length ) const override;

	// Called by the command system when argv(0) doesn't match a known command
	// Returns true if argv(0) is a variable reference and prints or changes the CVar
	virtual bool ExecuteCommand( const CCmdArgs& args ) override;

	// Sets/gets/clears modified flags that tell what kind of cvars have changed
	virtual void   SetModifiedFlags( uint32 flags ) override;
	virtual uint32 GetModifiedFlags() const override;
	virtual void   ClearModifiedFlags( uint32 flags ) override;

	// Resets variables with one of the given flags set
	virtual void ResetFlaggedVariables( uint32 flags ) override;

	// Writes variables with one of the given flags set to the given stream data.
	virtual void WriteFlaggedVariables( uint32 flags, IStreamDataWriter* pStreamData ) const override;

	CCVarSystem();

private:
	cvarDLLIdentifier_t nextDLLIdentifier;
	uint32				modifiedFlags;
	eastl::list<ICVar*> cvarList;
};

extern CCVarSystem s_cvarSystem;
