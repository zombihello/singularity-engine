#include "pch_tier1.h"
#include <EASTL/stack.h>

#include "tier1/filetools.h"
#include "tier1/keyvalues_parser.h"
#include "tier1/keyvalues_writer.h"
#include "tier1/keyvalues.h"

/*
==================
CKeyValues::RemoveAllSubKeys
==================
*/
void CKeyValues::RemoveAllSubKeys( bool bDelete /* = true */ )
{
	PROFILER_SCOPE_FUNC();
	if ( bDelete )
	{
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			// We set pParentKey to NULL because in the destructor we call DetachFromParent and
			// RemoveAllSubKeys to destroy all subkeys
			CKeyValues* pKeyValue = *it;
			pKeyValue->pParentKey = NULL;
			delete pKeyValue;
		}
	}
	subKeys.clear();
}

/*
==================
CKeyValues::FindKey
==================
*/
CKeyValues* CKeyValues::FindKey( const char* pName, bool bCreate /* = false */ )
{
	// Return the current key if a NULL subkey is asked for
	PROFILER_SCOPE_FUNC();
	if ( !pName || !pName[0] )
	{
		return this;
	}

	// Look for '/' characters delimiting sub fields
	const char* pSubStr	   = S_Strchr( pName, '/' );
	uint64		nameLength = pSubStr ? (uint64)( pSubStr - pName ) : S_Strlen( pName );

	// Find the searchStr in the current subKeys
	nameID_t				   searchNameID = pNamePool->Find( pName, nameLength );
	eastl::vector<CKeyValues*> keyValues;
	if ( searchNameID != (uint16)INVALID_INDEX )
	{
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			CKeyValues* pCurKeyValues = *it;
			if ( pCurKeyValues->nameID == searchNameID )
			{
				keyValues.emplace_back( pCurKeyValues );
				if ( !pSubStr )
				{
					break;
				}
			}
		}
	}

	// Make sure a key was found
	if ( keyValues.empty() )
	{
		if ( bCreate )
		{
			char* pSubKeyName = nameLength <= 255 ? (char*)Mem_Alloca( nameLength + 1 ) : (char*)Mem_Malloc( nameLength + 1 );
			Mem_Memcpy( pSubKeyName, pName, nameLength );
			pSubKeyName[nameLength] = '\0';
			keyValues.emplace_back( new CKeyValues( pSubKeyName, this ) );

			if ( nameLength > 255 )
			{
				delete pSubKeyName;
			}
		}
		else
		{
			return NULL;
		}
	}

	// If we have still got a pSubStr we need to keep looking deeper in the tree
	if ( pSubStr )
	{
		// Recursively chain down through the paths in the string
		CKeyValues* pKeyValues = NULL;
		for ( uint32 index = 0, count = (uint32)keyValues.size(); index < count && !pKeyValues; ++index )
		{
			pKeyValues = keyValues[index]->FindKey( pSubStr + 1, bCreate );
		}

		return pKeyValues;
	}
	return !keyValues.empty() ? keyValues[0] : NULL;
}

/*
==================
CKeyValues::SetParent
==================
*/
void CKeyValues::SetParent( CKeyValues* pParentKey )
{
	// Do nothing if we already have the parent
	PROFILER_SCOPE_FUNC();
	if ( CKeyValues::pParentKey == pParentKey )
	{
		return;
	}

#if ENABLE_ASSERT
	// We can't attach it to our own subkeys
	for ( CKeyValues* pKeyValues = pParentKey; pKeyValues; pKeyValues = pKeyValues->pParentKey )
	{
		Assert( pKeyValues != this );
	}
#endif	// ENABLE_ASSERT

	// Defining a new name pool
	namePool_t* pOldNamePool	= CKeyValues::pNamePool;
	bool		bOldOwnNamePool = bOwnNamePool;
	namePool_t* pNamePool		= NULL;
	if ( pParentKey )
	{
		// We become a child and must receive the parent's pool
		pNamePool = pParentKey->pNamePool;
		Assert( pNamePool );
	}
	else
	{
		// We become root and should get OUR own pool
		pNamePool = new namePool_t();
	}

	// We are mark the pool is our or not
	bOwnNamePool = !pParentKey;

	// We transfer all the children's names and our own to the new pool
	// if it has been changed
	if ( pOldNamePool != pNamePool )
	{
		MigrateNamePool( pNamePool );
	}

	// If we were previously root and now we are child, delete the old pool
	if ( bOldOwnNamePool && pOldNamePool && pOldNamePool != pNamePool )
	{
		delete pOldNamePool;
	}

	// Remove yourself from a list of the old parent
	if ( CKeyValues::pParentKey )
	{
		CKeyValues::pParentKey->subKeys.remove( this );
	}

	// Save our new parent
	CKeyValues::pParentKey = pParentKey;

	// Add yourself to a list of the new parent
	if ( pParentKey )
	{
		pParentKey->subKeys.emplace_back( this );
	}
}

/*
==================
CKeyValues::MigrateNamePool
==================
*/
void CKeyValues::MigrateNamePool( namePool_t* pNamePool )
{
	// We don't do anything if it's the same pool
	PROFILER_SCOPE_FUNC();
	Assert( pNamePool );
	if ( CKeyValues::pNamePool == pNamePool )
	{
		return;
	}

	// Iterative traversal to avoid recursion on large trees
	eastl::stack<CKeyValues*> stack;
	stack.push( this );
	while ( !stack.empty() )
	{
		CKeyValues* pKeyValue = stack.top();
		stack.pop();

		const char* pName	 = pKeyValue->pNamePool ? pKeyValue->pNamePool->GetString( pKeyValue->nameID ) : NULL;
		pKeyValue->nameID	 = pName ? pNamePool->FindOrAdd( pName, S_Strlen( pName ) ) : INVALID_INDEX;
		pKeyValue->pNamePool = pNamePool;

		for ( auto it = pKeyValue->subKeys.begin(), itEnd = pKeyValue->subKeys.end(); it != itEnd; ++it )
		{
			stack.push( *it );
		}
	}
}

/*
==================
CKeyValues::LoadFromStream
==================
*/
bool CKeyValues::LoadFromStream( IStreamDataReader* pStreamReader )
{
	// Do nothing if the stream isn't valid
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( pStreamReader );

	// Parse key values from the stream
	CKeyValuesParser keyValuesParser;
	keyValuesParser.Parse( this, pStreamReader );
	if ( keyValuesParser.HasErrors() )
	{
		const eastl::vector<eastl::string>& errorMsgs = keyValuesParser.GetErrorMsgs();
		const char*							pPath	  = pStreamReader->GetPath();
		Error( "KeyValues: Failed to load '%s', %i error(s)", pPath ? pPath : "<NO_PATH>", errorMsgs.size() );
		for ( uint32 index = 0, count = (uint32)errorMsgs.size(); index < count; ++index )
		{
			Error( "KeyValues: %s", errorMsgs[index].c_str() );
		}
	}

	return !keyValuesParser.HasErrors();
}

/*
==================
CKeyValues::SaveToStream
==================
*/
void CKeyValues::SaveToStream( IStreamDataWriter* pStreamWriter ) const
{
	// Do nothing if the stream isn't valid
	PROFILER_SCOPE_FUNC();
	Assert( pStreamWriter );

	// Write key values into the stream
	CKeyValuesWriter keyValuesWriter;
	keyValuesWriter.Write( (CKeyValues*)this, pStreamWriter );
}

/*
==================
CKeyValuesSubKeysIterator::Init
==================
*/
void CKeyValuesSubKeysIterator::Init( CKeyValues* pKeyValues, const char* pKeyName, bool bAllowValues, bool bAllowSubKeys, bool bAllowEmpty )
{
	// We iterate over all subkeys and filter out only the required ones
	PROFILER_SCOPE_FUNC();
	Assert( pKeyValues );

	// Reset key values in the iterator
	currentIndex = INVALID_INDEX;
	keyValues.clear();

	bool							bSetKeyName = pKeyName && pKeyName[0];
	CKeyValues::nameID_t			nameID		= bSetKeyName ? pKeyValues->pNamePool->Find( pKeyName, S_Strlen( pKeyName ) ) : INVALID_INDEX;
	const eastl::list<CKeyValues*>& subKeys		= pKeyValues->GetSubKeys();
	if ( !bSetKeyName || nameID != INVALID_INDEX )
	{
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			CKeyValues* pSubKey = *it;
			if ( bSetKeyName && pSubKey->nameID != nameID )
			{
				continue;
			}

			if ( ( bAllowValues && pSubKey->HasData() ) || ( bAllowSubKeys && pSubKey->HasSubKeys() ) || ( bAllowEmpty && pSubKey->IsEmpty() ) )
			{
				keyValues.emplace_back( pSubKey );
			}
		}
	}

	// Initialize the current index if key values aren't empty
	if ( !keyValues.empty() )
	{
		currentIndex = 0;
	}
}
