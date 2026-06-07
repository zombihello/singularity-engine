#include "pch_cvar.h"
#include "tier0/icommandline.h"
#include "cvar/cvars.h"
#include "cvar/cvarsystem.h"

CCVarSystem g_cvarSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCVarSystem, ICVarSystem, CVARSYSTEM_INTERFACE_VERSION, g_cvarSystem );

/*
==================
CCVarSystem::CCVarSystem
==================
*/
CCVarSystem::CCVarSystem()
	: nextDLLIdentifier( 0 )
	, modifiedFlags( CVAR_FLAG_NONE )
{
}

/*
==================
CCVarSystem::Connect
==================
*/
bool CCVarSystem::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC();
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	LinkCmds();
	LinkCVars();
	return true;
}

/*
==================
CCVarSystem::Disconnect
==================
*/
void CCVarSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC();
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();
}

/*
==================
CCVarSystem::Init
==================
*/
bool CCVarSystem::Init()
{
	PROFILER_SCOPE_FUNC();
	if ( CommandLine()->HasParam( "dev" ) )
	{
		developer.SetInt( 1 );
		cheats.SetInt( 1 );
	}
	return true;
}

/*
==================
CCVarSystem::Shutdown
==================
*/
void CCVarSystem::Shutdown()
{
	PROFILER_SCOPE_FUNC();
	UnregisterAllVariables();
	nextDLLIdentifier = 0;
	modifiedFlags	  = CVAR_FLAG_NONE;
}

/*
==================
CCVarSystem::AllocateDLLIdentifier
==================
*/
cvarDLLIdentifier_t CCVarSystem::AllocateDLLIdentifier()
{
	return nextDLLIdentifier++;
}

/*
==================
CCVarSystem::RegisterVariable
==================
*/
void CCVarSystem::RegisterVariable( ICVar* pCVar )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the cvar already is registered
	if ( pCVar->IsRegistered() )
	{
		return;
	}

	// Mark as registered
	pCVar->SetRegistered( true );

	// Do nothing if name isn't valid
	const char* pName = pCVar->GetName();
	if ( !pName || !pName[0] )
	{
		return;
	}

	// If the variable is already defined, then setup the new variable as a proxy to it
	ICVar* pParentCVar = ICVarSystem::FindVariable( pName );
	if ( pParentCVar )
	{
		// Make sure the default values are the same
		if ( pCVar->GetDefault() && pParentCVar->GetDefault() && S_Stricmp( pCVar->GetDefault(), pParentCVar->GetDefault() ) != 0 )
		{
			Warning( "CVarSystem: CVar '%s' has different default values (child: '%s', parent: '%s', parent wins)",
					 pCVar->GetName(), pCVar->GetDefault(), pParentCVar->GetDefault() );
		}

		// Transfer children's callbacks to parent
		if ( pCVar->GetNumChangeCallbacks() > 0 )
		{
			for ( uint32 index = 0, count = pCVar->GetNumChangeCallbacks(); index < count; ++index )
			{
				pParentCVar->InstallChangeCallback( pCVar->GetChangeCallback( index ), false );
			}

			// Wipe child callbacks
			pCVar->RemoveAllChangeCallbacks();
		}

		// Make sure we don't have conflicting description strings
		if ( pCVar->GetDescription() && S_Strlen( pCVar->GetDescription() ) > 0 )
		{
			if ( pParentCVar->GetDescription() && S_Strlen( pParentCVar->GetDescription() ) > 0 )
			{
				if ( S_Stricmp( pParentCVar->GetDescription(), pCVar->GetDescription() ) != 0 )
				{
					Warning( "CVarSystem: CVar '%s' has multiple description strings (parent (wins): '%s', child: '%s')",
							 pCVar->GetName(), pParentCVar->GetDescription(), pCVar->GetDescription() );
				}
			}
			else
			{
				pParentCVar->SetDescription( pCVar->GetDescription() );
			}
		}

		// Make sure we don't have conflicting CVAR_FLAG_*** flags
		static const uint32 s_cvarFlags[] = {
			CVAR_FLAG_CHEAT,
			CVAR_FLAG_ARCHIVE
		};
		static const char* s_pCVarFlagNames[] = {
			"CVAR_FLAG_CHEAT",
			"CVAR_FLAG_ARCHIVE"
		};
		static_assert( ARRAYSIZE( s_cvarFlags ) == ARRAYSIZE( s_pCVarFlagNames ), "Array size of s_cvarFlags must be equal to s_pCVarFlagNames" );

		for ( uint32 index = 0, count = ARRAYSIZE( s_cvarFlags ); index < count; ++index )
		{
			uint32		curFlag		 = s_cvarFlags[index];
			const char* pCurFlagName = s_pCVarFlagNames[index];
			if ( pCVar->IsFlagSet( curFlag ) != pParentCVar->IsFlagSet( curFlag ) )
			{
				Warning( "CVarSystem: Cvar '%s' has conflicting '%s' flag (child: %s%s, parent: %s%s, parent wins)",
						 pCVar->GetName(), pCurFlagName,
						 pCVar->IsFlagSet( curFlag ) ? "has " : "no ", pCurFlagName,
						 pParentCVar->IsFlagSet( curFlag ) ? "has " : "no ", pCurFlagName );
			}
		}

		// Set parent
		pCVar->SetParent( pParentCVar->GetParent() );
		return;
	}

	// Link the variable in
	cvarList.emplace_back( pCVar );
}

/*
==================
CCVarSystem::UnregisterVariable
==================
*/
void CCVarSystem::UnregisterVariable( ICVar* pCVar )
{
	PROFILER_SCOPE_FUNC();

	// Do nothing if the cvar isn't registered
	if ( !pCVar->IsRegistered() )
	{
		return;
	}

	// Remove the cvar
	pCVar->SetRegistered( false );
	cvarList.remove( pCVar );
}

/*
==================
CCVarSystem::UnregisterAllVariables
==================
*/
void CCVarSystem::UnregisterAllVariables( cmdDLLIdentifier_t dllIdentifier /* = INVALID_INDEX */ )
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = cvarList.begin(); it != cvarList.end(); )
	{
		ICVar* pCVar = *it;
		if ( dllIdentifier == INVALID_INDEX || pCVar->GetDLLIdentifier() == dllIdentifier )
		{
			pCVar->SetRegistered( false );
			it = cvarList.erase( it );
		}
		else
		{
			++it;
		}
	}
}

/*
==================
CCVarSystem::FindVariable
==================
*/
ICVar* CCVarSystem::FindVariable( const char* pName, uint32 length ) const
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = cvarList.begin(), itEnd = cvarList.end(); it != itEnd; ++it )
	{
		ICVar*		pCVar		= *it;
		const char* pCVarName	= pCVar->GetName();
		uint32		cvarNameLen = S_Strlen( pCVarName );
		if ( length == cvarNameLen && !S_Strnicmp( pName, pCVarName, length ) )
		{
			return pCVar;
		}
	}

	return NULL;
}

/*
==================
CCVarSystem::ExecuteCommand
==================
*/
bool CCVarSystem::ExecuteCommand( const CCmdArgs& args )
{
	ICVar* pCVar = ICVarSystem::FindVariable( args.Argv( 0 ) );
	if ( !pCVar )
	{
		return false;
	}

	// If we have only one argument - print the variable
	if ( args.Argc() == 1 )
	{
		const char* pDescription   = pCVar->GetDescription();
		uint32		descriptionLen = S_Strlen( pDescription );
		Msg( "CVarSystem: '%s' is:'%s' default:'%s'", pCVar->GetName(), pCVar->GetString(), pCVar->GetDefault() );
		if ( descriptionLen > 0 )
		{
			Msg( "CVarSystem:\t%s", pDescription );
		}
	}
	// Otherwise set the value
	else
	{
		pCVar->SetString( args.Args( 1, INVALID_INDEX ).c_str() );
	}

	return true;
}

/*
==================
CCVarSystem::SetModifiedFlags
==================
*/
void CCVarSystem::SetModifiedFlags( uint32 flags )
{
	modifiedFlags |= flags;
}

/*
==================
CCVarSystem::GetModifiedFlags
==================
*/
uint32 CCVarSystem::GetModifiedFlags() const
{
	return modifiedFlags;
}

/*
==================
CCVarSystem::ClearModifiedFlags
==================
*/
void CCVarSystem::ClearModifiedFlags( uint32 flags )
{
	modifiedFlags &= ~flags;
}

/*
==================
CCVarSystem::ResetFlaggedVariables
==================
*/
void CCVarSystem::ResetFlaggedVariables( uint32 flags )
{
	PROFILER_SCOPE_FUNC();
	for ( auto it = cvarList.begin(), itEnd = cvarList.end(); it != itEnd; ++it )
	{
		ICVar* pCVar = *it;
		if ( !pCVar->IsFlagSet( flags ) )
		{
			continue;
		}

		// Do nothing if the cvar already has the default value
		if ( !S_Stricmp( pCVar->GetDefault(), pCVar->GetString() ) )
		{
			continue;
		}

		// Otherwise reset it
		pCVar->Reset();
	}
}

/*
==================
CCVarSystem::WriteFlaggedVariables
==================
*/
void CCVarSystem::WriteFlaggedVariables( uint32 flags, IStreamDataWriter* pStreamData ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );

	// Write cvars into a string buffer
	eastl::string buffer;
	for ( auto it = cvarList.begin(), itEnd = cvarList.end(); it != itEnd; ++it )
	{
		// Skip cvars that haven't the flags
		ICVar* pCVar = *it;
		if ( !pCVar->IsFlagSet( flags ) )
		{
			continue;
		}

		buffer += S_Sprintf( "%s \"%s\"\n", pCVar->GetName(), pCVar->GetString() );
	}

	// Write the buffer into the stream data
	pStreamData->Write( buffer.data(), buffer.size() * sizeof( char ) );
}
