#include "pch_tier1.h"
#include <EASTL/stack.h>

#include "filesystem/ifilesystem.h"
#include "utils/interfaces/interfaces.h"
#include "tier1/keyvalues_parser.h"
#include "tier1/keyvalues.h"

/*
==================
CKeyValues::RemoveAllSubKeys
==================
*/
void CKeyValues::RemoveAllSubKeys( bool bDelete /* = true */ )
{
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
	if ( !pName || !pName[0] )
	{
		return this;
	}

	// Look for '/' characters delimiting sub fields
	const char* pSubStr	   = S_Strchr( pName, '/' );
	uint64		nameLength = pSubStr ? (uint64)( pSubStr - pName ) : S_Strlen( pName );

	// Find the searchStr in the current subKeys
	nameID_t	searchNameID = pNamePool->Find( pName, nameLength );
	CKeyValues* pKeyValues	 = NULL;
	if ( searchNameID != (uint16)INVALID_INDEX )
	{
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			CKeyValues* pCurKeyValues = *it;
			if ( pCurKeyValues->nameID == searchNameID )
			{
				pKeyValues = pCurKeyValues;
				break;
			}
		}
	}

	// Make sure a key was found
	if ( !pKeyValues )
	{
		if ( bCreate )
		{
			pKeyValues = new CKeyValues( pName, nameLength, this );
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
		return pKeyValues->FindKey( pSubStr + 1, bCreate );
	}
	return pKeyValues;
}

/*
==================
CKeyValues::SetParent
==================
*/
void CKeyValues::SetParent( CKeyValues* pParentKey )
{
	// Do nothing if we already have the parent
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
CKeyValues::LoadFromFile
==================
*/
bool CKeyValues::LoadFromFile( const char* pPath )
{
	// Do nothing if file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try open file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64 fileSize = pFile->GetSize();
	byte*  pBuffer	= (byte*)Mem_MallocZero( fileSize );

	// Serialize data to string buffer
	pFile->Read( pBuffer, fileSize );

	// Parse the key values
	CKeyValuesParser keyValuesParser;
	keyValuesParser.Parse( pPath, this, (char*)pBuffer, fileSize );
	if ( keyValuesParser.HasErrors() )
	{
		const eastl::vector<eastl::string>& errorMsgs = keyValuesParser.GetErrorMsgs();
		Error( "KeyValues: Failed to load '%s', %i error(s)", pPath, errorMsgs.size() );
		for ( uint32 index = 0, count = (uint32)errorMsgs.size(); index < count; ++index )
		{
			Error( "KeyValues: %s", errorMsgs[index].c_str() );
		}
	}

	// Free allocated memory
	Mem_Free( pBuffer );
	return !keyValuesParser.HasErrors();
}

/*
==================
CKeyValues::LoadFromBuffer
==================
*/
bool CKeyValues::LoadFromBuffer( const char* pBuffer, uint64 size )
{
	CKeyValuesParser keyValuesParser;
	keyValuesParser.Parse( "<buffer>", this, pBuffer, size );
	if ( keyValuesParser.HasErrors() )
	{
		const eastl::vector<eastl::string>& errorMsgs = keyValuesParser.GetErrorMsgs();
		Error( "KeyValues: Failed to load from buffer, %i error(s)", errorMsgs.size() );
		Error( "KeyValues:\n%.*s", size, pBuffer );
		Error( "KeyValues:" );
		for ( uint32 index = 0, count = (uint32)errorMsgs.size(); index < count; ++index )
		{
			Error( "KeyValues: %s", errorMsgs[index].c_str() );
		}
	}
	return !keyValuesParser.HasErrors();
}

/*
==================
CKeyValues::SaveToFile
==================
*/
bool CKeyValues::SaveToFile( const char* pPath ) const
{
	// Do nothing if file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Save data to buffer
	eastl::string buffer;
	if ( !SaveToBuffer( buffer ) )
	{
		Warning( "KeyValues: Failed to save KeyValues into buffer" );
		return false;
	}

	// Try open file for save
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "KeyValues: Failed to create file '%s'", pPath );
		return false;
	}

	// Serialize buffer to the file
	pFile->Write( buffer.data(), buffer.size() * sizeof( char ) );
	return true;
}

/*
==================
CKeyValues::SaveToBuffer
==================
*/
bool CKeyValues::SaveToBuffer( eastl::string& buffer ) const
{
	AssertUnimplemented();
	return false;
}

/*
==================
CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator
==================
*/
CKeyValuesSubKeysIterator::CKeyValuesSubKeysIterator( CKeyValues* pKeyValues, bool bWithValues /* = true */, bool bWithSubKeys /* = false */ )
	: currentIndex( INVALID_INDEX )
{
	// We iterate over all subkeys and filter out only the required ones
	PROFILE_SCOPE();
	Assert( pKeyValues );
	const eastl::list<CKeyValues*>& subKeys = pKeyValues->GetSubKeys();
	for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
	{
		CKeyValues* pSubKey = *it;
		if ( ( pSubKey->HasData() && bWithValues ) || ( pSubKey->HasSubKeys() && bWithSubKeys ) )
		{
			keyValues.emplace_back( pSubKey );
		}
	}

	// Initialize the current index if key values aren't empty
	if ( !subKeys.empty() )
	{
		currentIndex = 0;
	}
}
