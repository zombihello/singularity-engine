#include "utils/interfaces/interfaces.h"
#include "tier0/profile.h"
#include "tier1/keyvalues.h"
#include "filesystem/ifilesystem.h"
#include "utils/smapdoc/smap_source_doc.h"

/*
==================
CSMAPSourceMapDoc::LoadFromFile
==================
*/
bool CSMAPSourceMapDoc::LoadFromFile( const char* pPath )
{
	// Load key values file
	PROFILE_SCOPE();
	CKeyValues keyValues( "smap" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}
	Clear();

	// Get each entity
	for ( CKeyValuesSubKeysIterator it( &keyValues, false, true, true ); it; ++it )
	{
		CSMAPEntity smapEntity;
		bool		bGotDefaultValue = false;
		const char* pName			 = it->GetName();
		const char* pClassName		 = it->GetString( "classname", "", NULL, &bGotDefaultValue );
		if ( bGotDefaultValue )
		{
			Error( "SMAPDoc: Invalid SMAP, not found required field 'classname' in entity '%s' (file: '%s')", pName, pPath );
			return false;
		}
		if ( !pClassName || !pClassName[0] )
		{
			Error( "SMAPDoc: Invalid SMAP, empty class name in '%s' (file: '%s')", pName, pPath );
			return false;
		}

		// Set an entity and class name
		smapEntity.SetName( pName );
		smapEntity.SetClassName( pClassName );

		// Add the entity into an array
		entities.emplace_back( smapEntity );
	}

	// We are done
	return true;
}

/*
==================
CSMAPSourceMapDoc::SaveFile
==================
*/
bool CSMAPSourceMapDoc::SaveFile( const char* pPath )
{
	// Create key values
	PROFILE_SCOPE();
	CKeyValues keyValues( "smap" );
	if ( !entities.empty() )
	{
		for ( uint32 entityIdx = 0, numEntities = (uint32)entities.size(); entityIdx < numEntities; ++entityIdx )
		{
			const CSMAPEntity& smapEntity = entities[entityIdx];
			CKeyValues*		   pEntity	  = new CKeyValues( smapEntity.GetName(), &keyValues );
			pEntity->SetString( "classname", smapEntity.GetClassName() );
		}
	}

	// Save the key values to a file
	return keyValues.SaveToFile( pPath );
}
