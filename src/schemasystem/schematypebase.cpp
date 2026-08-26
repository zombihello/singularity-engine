#include "pch_schemasystem.h"
#include "schemasystem/schematypebase.h"

#if ENABLE_SCHEMA_METADATA
/*
==================
CSchemaMetadata::Init
==================
*/
void CSchemaMetadata::Init( const schemaMetadataDesc_t* pMetadata, uint32 numMetadata )
{
	// Clear old metadata
	PROFILER_SCOPE_FUNC();
	metadata.clear();
	metadataDict.clear();

	// Add each metadata
	metadata.reserve( numMetadata );
	metadataDict.reserve( numMetadata );
	for ( uint32 index = 0; index < numMetadata; ++index )
	{
		const schemaMetadataDesc_t& metadataDesc = pMetadata[index];
		metadata.emplace_back( metadataDesc );
		metadataDict.emplace( metadataDesc.pKey, (uint32)metadata.size() - 1 );
	}
}

/*
==================
CSchemaMetadata::FindValue
==================
*/
const char* CSchemaMetadata::FindValue( const char* pKey, const char* pDefaultValue /* = "" */ ) const
{
	PROFILER_SCOPE_FUNC();
	auto it = metadataDict.find( pKey );
	if ( it != metadataDict.end() )
	{
		return metadata[it->second].pValue;
	}
	return pDefaultValue;
}

/*
==================
CSchemaMetadata::GetNum
==================
*/
uint32 CSchemaMetadata::GetNum() const
{
	return (uint32)metadata.size();
}

/*
==================
CSchemaMetadata::GetKey
==================
*/
const char* CSchemaMetadata::GetKey( uint32 index ) const
{
	Assert( index < (uint32)metadata.size() );
	return metadata[index].pKey;
}

/*
==================
CSchemaMetadata::GetKey
==================
*/
const char* CSchemaMetadata::GetValue( uint32 index ) const
{
	Assert( index < (uint32)metadata.size() );
	return metadata[index].pValue;
}
#endif	// ENABLE_SCHEMA_METADATA
