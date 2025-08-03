#include "pch_cvar.h"
#include "filesystem/ifilesystem.h"
#include "cvar/icvar.h"
#include "stdlib/convar.h"

#define CVAR_CONFIG_NAME				"config"
#define CVAR_DEFAULT_CONFIG_NAME		"config_default"


//-----------------------------------------------------------------------------
// Cvar delegates
//-----------------------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE( COnWriteConCmdsToConfigFile, IStreamDataWriter* /* pStreamData */ );


//-----------------------------------------------------------------------------
// Cheat cvar
//-----------------------------------------------------------------------------
/*
==================
CheatsChanged
==================
*/
static void CheatsChanged( IConVar* pConVar )
{
	PROFILE_SCOPE();

	// Cheats were disabled, revert all cheat cvars to their default values
	if ( g_pCvar && pConVar->GetInt() == 0 )
	{
		g_pCvar->ResetFlaggedVars( FCVAR_CHEAT );
		Msg( "FCVAR_CHEAT cvars reverted to defaults" );
	}
}
CConVar		cheats( "cheats", "0", "Allow cheats in the game", FCVAR_NONE, CheatsChanged );


//-----------------------------------------------------------------------------
// CvarQuery implementation
//-----------------------------------------------------------------------------
class CCvarQuery : public CBaseAppSystem<ICvarQuery> 
{
public:
	CCvarQuery();

	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	// ICvarQuery interface
	virtual bool AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar ) override;

private:
	ICvar*		pCvar;
};


//-----------------------------------------------------------------------------
// The console system
//-----------------------------------------------------------------------------
class CCvar : public CBaseAppSystem<ICvar>
{
public:
	CCvar();

	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;
	virtual void Shutdown() override;

	// ICvar interface
	virtual cvarDLLIdentifier_t AllocateDLLIdentifier() override;

	virtual void RegisterCommand( IConCmdBase* pCommand ) override;
	virtual void UnregisterCommand( IConCmdBase* pCommand ) override;
	virtual void UnregisterCommands( cvarDLLIdentifier_t dllIdentifier ) override;

	virtual bool Exec( const achar* pCommand ) override;

	virtual IConCmdBase* FindCommandBase( const achar* pName ) const override;
	virtual IConCmd* FindCommand( const achar* pName ) const override;
	virtual IConVar* FindVar( const achar* pName ) const override;

	// Read and write a configuration file
	virtual void ReadConfigFile( const achar* pConfigDir, bool bWriteConfigIfNotExist = true ) override;
	virtual void WriteConfigFile( const achar* pConfigDir, bool bWriteDefaultConfig = false ) override;
	virtual IOnWriteConCmdsToConfigFile* OnWriteConCmdsToConfigFile() const override;

	// Override IConVars from a command line
	virtual void OverrideConVarsFromCommandLine() override;
	virtual void SetConVarsOverrider( cvarDLLIdentifier_t dllIdentifier, IConVarsOverrider* pConVarsOverrider ) override;

	// Sets cvars containing the flags to their default value
	virtual void ResetFlaggedVars( uint32 flags ) override;

	// Get the first IConCmdBase to allow iteration over all IConCmd and IConVars
	virtual IConCmdBase* GetCommands() const override;

	virtual void SetGlobalChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn ) override;
	virtual void CallGlobalChangeCallback( IConVar* pConVar ) override;

	virtual void SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc ) override;
	virtual void ConsolePrintf( const CColor& color, const achar* pFormat, ... ) override;
	virtual void ConsolePrintf( const achar* pFormat, ... ) override;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICvar, so we can't use the standard connect pattern
	// NOTE: pCvarQuery If NULL ICvar set to default ICvarQuery
	virtual void SetCVarQuery( ICvarQuery* pCvarQuery ) override;

private:
	enum
	{
		COMMAND_MAX_ARGC		= 64,
		COMMAND_MAX_LENGTH		= 512
	};

	// This method parse command and result place into variables:
	// commandArgc - Here will be count command arguments
	// pCommandArgv - Here will be command arguments
	// NOTE: pCommand - After the function is executed, this pointer will point to the beginning 
	// of the next command after the separator, or NULL if there is nothing else left
	bool ParseCommand( const achar*& pCommand, const achar separator = '$' );

	IConCmdBase*													pConCmdList;
	ICvarQuery*														pCvarQuery;
	cvarDLLIdentifier_t												nextDLLIdentifier;
	conVarChangeCallbackFn_t										pGlobalChangeCallbackFn;
	IConsoleDisplayFunc*											pConsoleDisplayFunc;
	achar															commandArgvBuffer[COMMAND_MAX_LENGTH];
	uint32															commandArgc;
	const achar*													pCommandArgv[COMMAND_MAX_ARGC];
	std::unordered_map<cvarDLLIdentifier_t, IConVarsOverrider*>		conVarsOverriderDict;
	COnWriteConCmdsToConfigFile										onWriteConCmdsToConfigFile;
};

static CCvar		s_Cvar;
static CCvarQuery	s_CvarQuery;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCvar, ICvar, CVAR_INTERFACE_VERSION, s_Cvar );
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CCvarQuery, ICvarQuery, CVAR_QUERY_INTERFACE_VERSION, s_CvarQuery );


/*
==================
CCvarQuery::CCvarQuery
==================
*/
CCvarQuery::CCvarQuery()
	: pCvar( NULL )
{}

/*
==================
CCvarQuery::Connect
==================
*/
bool CCvarQuery::Connect( createInterfaceFn_t pFactory )
{
	pCvar = ( ICvar* )pFactory( CVAR_INTERFACE_VERSION );
	if ( !pCvar )
	{
		return false;
	}

	pCvar->SetCVarQuery( this );
	return true;
}

/*
==================
CCvarQuery::Disconnect
==================
*/
void CCvarQuery::Disconnect()
{
	if ( pCvar )
	{
		pCvar->SetCVarQuery( NULL );
	}
}

/*
==================
CCvarQuery::QueryInterface
==================
*/
void* CCvarQuery::QueryInterface( const achar* pInterfaceName )
{
	if ( !S_Stricmp( pInterfaceName, CVAR_QUERY_INTERFACE_VERSION ) )
	{
		return ( ICvarQuery* )this;
	}

	return NULL;
}

/*
==================
CCvarQuery::AreConVarsLinkable
==================
*/
bool CCvarQuery::AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar )
{
	return true;
}


/*
==================
CCvar::CCvar
==================
*/
CCvar::CCvar()
	: pConCmdList( NULL )
	, pCvarQuery( &s_CvarQuery )
	, nextDLLIdentifier( 0 )
	, pGlobalChangeCallbackFn( NULL )
	, pConsoleDisplayFunc( NULL )
	, commandArgc( 0 )
{}

/*
==================
CCvar::Connect
==================
*/
bool CCvar::Connect( createInterfaceFn_t pFactory )
{
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	ConVar_Register();
	return true;
}

/*
==================
CCvar::Disconnect
==================
*/
void CCvar::Disconnect()
{
	ConVar_Unregister();
	DisconnectStdLib();
}

/*
==================
CCvar::Shutdown
==================
*/
void CCvar::Shutdown()
{
	nextDLLIdentifier = 0;
	conVarsOverriderDict.clear();
}

/*
==================
CCvar::AllocateDLLIdentifier
==================
*/
cvarDLLIdentifier_t CCvar::AllocateDLLIdentifier()
{
	return nextDLLIdentifier++;
}

/*
==================
CCvar::RegisterCommand
==================
*/
void CCvar::RegisterCommand( IConCmdBase* pCommand )
{
	PROFILE_SCOPE();

	// Do nothing if command already is registered
	if ( pCommand->IsRegistered() )
	{
		return;
	}

	// Mark as registered
	pCommand->SetRegistered( true );

	// Do nothing if name isn't valid
	const achar*	pName = pCommand->GetName();
	if ( !pName || !pName[0] )
	{
		pCommand->SetNext( NULL );
		return;
	}

	// If the variable is already defined, then setup the new variable as a proxy to it
	const IConCmdBase*	pOtherCommand = FindVar( pName );
	if ( pOtherCommand )
	{
		// We unable link commands, only variables
		if ( pCommand->IsCommand() || pOtherCommand->IsCommand() )
		{
			Warning( "Cvar: Unable to link %s and %s because one or more is a command", pName, pOtherCommand->GetName() );
		}
		// Otherwise link variables
		else
		{
			// This cast is ok because we make sure they're IConVars above
			IConVar*	pChildVar = ( IConVar* )pCommand;
			IConVar*	pParentVar = ( IConVar* )pOtherCommand;

			// See if it's a valid linkage
			if ( pCvarQuery->AreConVarsLinkable( pChildVar, pParentVar ) )
			{
				// Make sure the default values are the same
				if ( pChildVar->GetDefault() && pParentVar->GetDefault() && S_Stricmp( pChildVar->GetDefault(), pParentVar->GetDefault() ) != 0 )
				{
					Warning( "Cvar: Parent and child IConVars with different default values! %s child: %s parent: %s (parent wins)",
							 pCommand->GetName(), pChildVar->GetDefault(), pParentVar->GetDefault() );
				}

				// Set parent
				pChildVar->SetParent( pParentVar->GetParent() );

				// Check the parent's callbacks and slam if doesn't have, warn if both have callbacks
				if ( pChildVar->GetChangeCallback() )
				{
					if ( !pParentVar->GetChangeCallback() )
					{
						pParentVar->SetChangeCallback( pChildVar->GetChangeCallback() );
					}
					else
					{
						Warning( "Cvar: IConVar %s has multiple different change callbacks", pCommand->GetName() );
					}
				}

				// Make sure we don't have conflicting help strings
				if ( pChildVar->GetHelpText() && S_Strlen( pChildVar->GetHelpText() ) > 0 )
				{
					if ( pParentVar->GetHelpText() && S_Strlen( pParentVar->GetHelpText() ) > 0 )
					{
						if ( S_Stricmp( pParentVar->GetHelpText(), pChildVar->GetHelpText() ) != 0 )
						{
							Warning( "Cvar: IConVar %s has multiple help strings:\n\tparent (wins): \"%s\"\n\tchild: \"%s\"",
									 pCommand->GetName(), pParentVar->GetHelpText(), pChildVar->GetHelpText() );
						}
					}
					else
					{
						pParentVar->SetHelpText( pChildVar->GetHelpText() );
					}
				}

				// Make sure we don't have conflicting FCVAR_CHEAT flag
				if ( pChildVar->IsFlagSet( FCVAR_CHEAT ) != pParentVar->IsFlagSet( FCVAR_CHEAT ) )
				{
					Warning( "Cvar: IConVar %s has conflicting FCVAR_CHEAT flags (child: %s, parent: %s, parent wins)",
							 pCommand->GetName(), pChildVar->IsFlagSet( FCVAR_CHEAT ) ? "FCVAR_CHEAT" : "no FCVAR_CHEAT",
							 pParentVar->IsFlagSet( FCVAR_CHEAT ) ? "FCVAR_CHEAT" : "no FCVAR_CHEAT" );
				}
			}
		}

		pCommand->SetNext( NULL );
		return;
	}

	// Link the variable in
	pCommand->SetNext( pConCmdList );
	pConCmdList = pCommand;
}

/*
==================
CCvar::UnregisterCommand
==================
*/
void CCvar::UnregisterCommand( IConCmdBase* pCommand )
{
	PROFILE_SCOPE();

	// Do nothing if command isn't registered
	if ( !pCommand->IsRegistered() )
	{
		return;
	}

	// Unmark as registered
	pCommand->SetRegistered( false );

	// Remove command
	for ( IConCmdBase* pCurCommand = pConCmdList, *pPrevCommand = NULL; pCurCommand; pCurCommand = pCurCommand->GetNext() )
	{
		if ( pCurCommand != pCommand )
		{
			pPrevCommand = pCurCommand;
			continue;
		}

		if ( !pPrevCommand )
		{
			pConCmdList = pCurCommand->GetNext();
		}
		else
		{
			pPrevCommand->SetNext( pCurCommand->GetNext() );
		}

		pCurCommand->SetNext( NULL );
		break;
	}
}

/*
==================
CCvar::UnregisterCommands
==================
*/
void CCvar::UnregisterCommands( cvarDLLIdentifier_t dllIdentifier )
{
	PROFILE_SCOPE();
	IConCmdBase*	pNewList = NULL;
	IConCmdBase*	pCurCommand = pConCmdList, *pNextCommand = NULL;
	while ( pCurCommand )
	{
		pNextCommand = pCurCommand->GetNext();
		if ( pCurCommand->GetDLLIdentifier() != dllIdentifier )
		{
			pCurCommand->SetNext( pNewList );
			pNewList = pCurCommand;
		}
		else
		{
			pCurCommand->SetRegistered( false );
			pCurCommand->SetNext( NULL );
		}

		pCurCommand = pNextCommand;
	}

	pConCmdList = pNewList;
}

/*
==================
CCvar::ParseCommand
==================
*/
bool CCvar::ParseCommand( const achar*& pCommand, const achar separator /*= '$'*/ )
{
	PROFILE_SCOPE();

	// Reset command buffers
	commandArgc				= 0;
	commandArgvBuffer[0]	= '\0';

	// Do nothing if the command isn't valid
	if ( !pCommand || !pCommand[0] )
	{
		return false;
	}

	// Skip separators and spaces
	while ( *pCommand && ( *pCommand == separator || S_IsSpace( *pCommand ) ) )
	{
		++pCommand;
	}

	// Find the end of the command
	const achar*	pEndCommand	= pCommand;
	{
		bool	bFoundQuote = false;
		for ( ; *pEndCommand; ++pEndCommand )
		{
			// We must ignore a separator in quotes
			if ( *pEndCommand == '\"' )
			{
				bFoundQuote = !bFoundQuote;
				continue;
			}

			// We found a separator, it is the end
			if ( !bFoundQuote && *pEndCommand == separator )
			{
				break;
			}
		}
	}

	// Move pEndCommand to nearest symbol for save buffer size
	const achar*	pNextCommand = pEndCommand;
	while ( pEndCommand != pCommand && ( S_IsSpace( *pEndCommand ) || *pEndCommand == separator ) )
	{
		if ( !S_IsSpace( *( pEndCommand-1 ) ) && *( pEndCommand-1 ) != separator )
		{
			break;
		}
		--pEndCommand;
	}

	// Copy the current command into commandArgvBuffer
	uint32		commandLength = ( uint32 )( pEndCommand - pCommand );
	if ( commandLength >= COMMAND_MAX_LENGTH - 1 )
	{
		Warning( "Cvar: Encountered command which overflows the buffer, skipping!" );
		return false;
	}

	Mem_Memcpy( &commandArgvBuffer[0], pCommand, commandLength );
	commandArgvBuffer[commandLength]	= '\0';
	pCommand							= pNextCommand;

	// Parse the current command into pCommandArgv
	achar*	pStartArgv		= &commandArgvBuffer[0];
	achar*	pCurChar		= pStartArgv;
	bool	bFoundQuote		= false;
	bool	bEndSubstring	= false;
	while ( *pCurChar && commandArgc < COMMAND_MAX_ARGC )
	{
		// We found a quote
		if ( *pCurChar == '\"' )
		{
			bFoundQuote = !bFoundQuote;
			if ( bFoundQuote )
			{
				// The quote must be at begin of the argv, otherwise it's syntax error
				if ( pCurChar == pStartArgv )
				{
					++pStartArgv;
				}
				else
				{
					Warning( "Cvar: Encountered command have wrong syntax" );
					return false;
				}
			}
			else
			{
				if ( pCurChar != pStartArgv )
				{
					--pCurChar;
				}
				bEndSubstring = true;
			}
		}
		// Otherwise if next will be a space or the end of string then also we found another argv
		else if ( !bFoundQuote && S_IsSpace( *( pCurChar+1 ) ) || !*( pCurChar+1 ) )
		{
			bEndSubstring = true;
		}

		// We found substring save pointer into pCommandArgv
		if ( bEndSubstring )
		{
			bEndSubstring				= false;
			pCommandArgv[commandArgc]	= pStartArgv;
			++commandArgc;
			if ( commandArgc >= COMMAND_MAX_ARGC )
			{
				Warning( "Cvar: Encountered command which overflows the argument buffer, clamped!" );
			}

			// Set in the argv null terminator if it need
			if ( *( pCurChar+1 ) )
			{
				++pCurChar;
				*pCurChar = '\0';
			}
			++pCurChar;

			// Eat white space
			while ( *pCurChar && S_IsSpace( *pCurChar ) )
			{
				++pCurChar;
			}
			pStartArgv = pCurChar;
			
		}
		else
		{
			++pCurChar;
		}
	}

	return true;
}

/*
==================
CCvar::Exec
==================
*/
bool CCvar::Exec( const achar* pCommand )
{
	PROFILE_SCOPE();

	// Do nothing if a command is empty
	if ( !pCommand || !pCommand[0] )
	{
		return false;
	}

	// Execute the command
	bool	bResult = false;
	while ( pCommand && *pCommand != '\0' )
	{
		// Parse the command
		if ( ParseCommand( pCommand ) && commandArgc > 0 )
		{
			// Find a command
			IConCmdBase*	pConCmdBase = FindCommandBase( pCommandArgv[0] );
			if ( pConCmdBase )
			{
				// Allow cheat commands with 'cheats' on
				if ( pConCmdBase->IsFlagSet( FCVAR_CHEAT ) && !cheats.GetBool() && ( pConCmdBase->IsCommand() || commandArgc > 1 ) )
				{
					Warning( "Cvar: Can't use cheat command \"%s\", unless has cheats set to 1", pCommandArgv[0] );
					continue;
				}

				// Execute if it is command
				if ( pConCmdBase->IsCommand() )
				{
					IConCmd*	pConCmd = ( IConCmd* )pConCmdBase;
					pConCmd->Exec( commandArgc - 1, pCommandArgv + 1 );
				}
				// Otherwise it's variable
				else 
				{
					// Change variable if we have enough arguments
					IConVar*	pConVar = ( IConVar* )pConCmdBase;
					if ( commandArgc > 1 )
					{
						pConVar->SetString( pCommandArgv[1] );
					}
					// Otherwise print current and default value
					else
					{
						Msg( "Cvar: %s (%s): Current \"%s\", Default: \"%s\"", pCommandArgv[0], pConVar->GetHelpText(), pConVar->GetString(), pConVar->GetDefault() );
					}
				}

				bResult |= true;
			}
			else
			{
				Warning( "Cvar: Unknown command \"%s\"", pCommandArgv[0] );
			}
		}
	}
	
	return bResult;
}

/*
==================
CCvar::FindCommandBase
==================
*/
IConCmdBase* CCvar::FindCommandBase( const achar* pName ) const
{
	PROFILE_SCOPE();
	for ( IConCmdBase* pCurCommand = pConCmdList; pCurCommand; pCurCommand = pCurCommand->GetNext() )
	{
		if ( !S_Stricmp( pName, pCurCommand->GetName() ) )
		{
			return pCurCommand;
		}
	}

	return NULL;
}

/*
==================
CCvar::FindCommand
==================
*/
IConCmd* CCvar::FindCommand( const achar* pName ) const
{
	PROFILE_SCOPE();
	IConCmdBase*	pConCmd = FindCommandBase( pName );
	if ( !pConCmd || !pConCmd->IsCommand() )
	{
		return NULL;
	}

	return ( IConCmd* )pConCmd;
}

/*
==================
CCvar::FindVar
==================
*/
IConVar* CCvar::FindVar( const achar* pName ) const
{
	PROFILE_SCOPE();
	IConCmdBase*	pConVar = FindCommandBase( pName );
	if ( !pConVar || pConVar->IsCommand() )
	{
		return NULL;
	}

	return ( IConVar* )pConVar;
}

/*
==================
CCvar::ReadConfigFile
==================
*/
void CCvar::ReadConfigFile( const achar* pConfigDir, bool bWriteConfigIfNotExist /* = true */ )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	std::string		configPath = S_Sprintf( "%s/" CVAR_CONFIG_NAME ".cfg", pConfigDir );
	if ( g_pFileSystem->IsFileExists( configPath.c_str() ) )
	{
		g_pCvar->Exec( S_Sprintf( "exec %s", configPath.c_str() ).c_str() );
		return;
	}

	// Otherwise try execute a default config if the one is exist
	configPath = S_Sprintf( "%s/" CVAR_DEFAULT_CONFIG_NAME ".cfg", pConfigDir );
	if ( g_pFileSystem->IsFileExists( configPath.c_str() ) )
	{
		g_pCvar->Exec( S_Sprintf( "exec %s", configPath.c_str() ).c_str() );
	}

	// Save a new config if it need
	if ( bWriteConfigIfNotExist )
	{
		WriteConfigFile( pConfigDir );
	}
}

/*
==================
CCvar::WriteConfigFile
==================
*/
void CCvar::WriteConfigFile( const achar* pConfigDir, bool bWriteDefaultConfig /* = false */ )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Open file to write
	std::string					configPath = S_Sprintf( !bWriteDefaultConfig ? "%s/" CVAR_CONFIG_NAME ".cfg" : "%s/" CVAR_DEFAULT_CONFIG_NAME ".cfg", pConfigDir );
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( configPath.c_str() );
	if ( !pFile )
	{
		Warning( "Cvar: Failed to create file configuration '%s'", configPath.c_str() );
		return;
	}

	// Broadcast an event that causes other subsystems to write concmds to the file (i.g: 'bind q quit', 'exec subconfig.cfg')
	onWriteConCmdsToConfigFile.Broadcast( pFile );

	// Write cvars
	std::string		buffer;
	for ( IConCmdBase* pVar = g_pCvar->GetCommands(); pVar; pVar = pVar->GetNext() )
	{
		// Skip commands and cvars that not have FCVAR_ARCHIVE
		if ( pVar->IsCommand() || !pVar->IsFlagSet( FCVAR_ARCHIVE ) )
		{
			continue;;
		}

		buffer += S_Sprintf( "%s \"%s\"\n", pVar->GetName(), ( ( IConVar* )pVar )->GetString() );
	}

	// Write the buffer into the file
	pFile->Write( buffer.data(), buffer.size() * sizeof( achar ) );
}

/*
==================
CCvar::OnWriteConfiguration
==================
*/
IOnWriteConCmdsToConfigFile* CCvar::OnWriteConCmdsToConfigFile() const
{
	return ( IOnWriteConCmdsToConfigFile* )&onWriteConCmdsToConfigFile;
}

/*
==================
CCvar::OverrideConVarsFromCommandLine
==================
*/
void CCvar::OverrideConVarsFromCommandLine()
{
	for ( auto it = conVarsOverriderDict.begin(), itEnd = conVarsOverriderDict.end(); it != itEnd; ++it )
	{
		it->second->OverrideFromCommandLine();
	}
}

/*
==================
CCvar::SetConVarsOverrider
==================
*/
void CCvar::SetConVarsOverrider( cvarDLLIdentifier_t dllIdentifier, IConVarsOverrider* pConVarsOverrider )
{
	if ( !pConVarsOverrider )
	{
		conVarsOverriderDict.erase( dllIdentifier );
		return;
	}

	conVarsOverriderDict[dllIdentifier] = pConVarsOverrider;
}

/*
==================
CCvar::ResetFlaggedVars
==================
*/
void CCvar::ResetFlaggedVars( uint32 flags )
{
	PROFILE_SCOPE();
	for ( IConCmdBase* pCurVar = pConCmdList; pCurVar; pCurVar = pCurVar->GetNext() )
	{
		// Skip commands and cvars that no have a specific flags
		if ( pCurVar->IsCommand() || !pCurVar->IsFlagSet( flags ) )
		{
			continue;
		}

		// Do nothing if cvar already have the default value
		IConVar*	pCvar = ( IConVar* )pCurVar;
		if ( !S_Stricmp( pCvar->GetDefault(), pCvar->GetString() ) )
		{
			continue;
		}

		// Otherwise reset it
		pCvar->Reset();
	}
}

/*
==================
CCvar::GetCommands
==================
*/
IConCmdBase* CCvar::GetCommands() const
{
	return pConCmdList;
}

/*
==================
CCvar::SetGlobalChangeCallback
==================
*/
void CCvar::SetGlobalChangeCallback( conVarChangeCallbackFn_t pChangeCallbackFn )
{
	pGlobalChangeCallbackFn = pChangeCallbackFn;
}

/*
==================
CCvar::CallGlobalChangeCallback
==================
*/
void CCvar::CallGlobalChangeCallback( IConVar* pConVar )
{
	PROFILE_SCOPE();
	if ( pGlobalChangeCallbackFn )
	{
		pGlobalChangeCallbackFn( pConVar );
	}
}

/*
==================
CCvar::SetConsoleDisplayFunc
==================
*/
void CCvar::SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc )
{
	CCvar::pConsoleDisplayFunc = pConsoleDisplayFunc;
}

/*
==================
CCvar::ConsolePrintf
==================
*/
void CCvar::ConsolePrintf( const CColor& color, const achar* pFormat, ... )
{
	if ( pConsoleDisplayFunc )
	{
		va_list		params;
		va_start( params, pFormat );
		pConsoleDisplayFunc->Print( S_Vsprintf( pFormat, params ).c_str(), color );
		va_end( params );
	}
}

/*
==================
CCvar::ConsolePrintf
==================
*/
void CCvar::ConsolePrintf( const achar* pFormat, ... )
{
	if ( pConsoleDisplayFunc )
	{
		va_list		params;
		va_start( params, pFormat );
		pConsoleDisplayFunc->Print( S_Vsprintf( pFormat, params ).c_str() );
		va_end( params );
	}
}

/*
==================
CCvar::SetCVarQuery
==================
*/
void CCvar::SetCVarQuery( ICvarQuery* pCvarQuery )
{
	CCvar::pCvarQuery = pCvarQuery ? pCvarQuery : &s_CvarQuery;
}