#include "pch_schemasystem.h"
#include "schemasystem/schemaenum.h"

/*
==================
CSchemaEnum::CSchemaEnum
==================
*/
CSchemaEnum::CSchemaEnum( const schemaEnumDesc_t& enumDesc )
	: CSchemaTypeBase<ISchemaEnum>( SCHEMA_TYPE_KIND_ENUM, enumDesc.pName, enumDesc.pAliasName, enumDesc.flags, enumDesc.typeSize, enumDesc.alignment, enumDesc.pMetadata, enumDesc.numMetadata )
	, underlyingType( enumDesc.underlyingType )
{
	values.reserve( enumDesc.numValues );
	valueDict.reserve( enumDesc.numValues );
	for ( uint32 index = 0; index < enumDesc.numValues; ++index )
	{
		// Add a value and its name
		const schemaEnumValueDesc_t& valueDesc = enumDesc.pValues[index];
		values.emplace_back( valueDesc );
		valueDict.emplace( valueDesc.pName, index );
		nameDict.emplace( valueDesc.value, index );

		// Add an alias name if the name is free
		if ( valueDesc.pAliasName && valueDesc.pAliasName[0] )
		{
			if ( valueDict.find( valueDesc.pAliasName ) == valueDict.end() )
			{
				valueDict.emplace( valueDesc.pAliasName, index );
			}
			else
			{
				DevWarning( "SchemaSystem: The value '%s' of '%s' is registered without its alias name '%s', the name is already taken", valueDesc.pName, GetName(), valueDesc.pAliasName );
			}
		}
	}
}

/*
==================
CSchemaEnum::FindValueByName
==================
*/
bool CSchemaEnum::FindValueByName( const char* pName, int64& value ) const
{
	auto it = valueDict.find( pName );
	if ( it == valueDict.end() )
	{
		value = INVALID_INDEX;
		return false;
	}

	value = values[it->second].value;
	return true;
}

/*
==================
CSchemaEnum::FindNameByValue
==================
*/
const char* CSchemaEnum::FindNameByValue( int64 value ) const
{
	auto it = nameDict.find( value );
	return it != nameDict.end() ? values[it->second].pName : "";
}

/*
==================
CSchemaEnum::GetUnderlyingType
==================
*/
schemaFieldType_t CSchemaEnum::GetUnderlyingType() const
{
	return underlyingType;
}

/*
==================
CSchemaEnum::GetNumValues
==================
*/
uint32 CSchemaEnum::GetNumValues() const
{
	return (uint32)values.size();
}

/*
==================
CSchemaEnum::GetValueName
==================
*/
const char* CSchemaEnum::GetValueName( uint32 index ) const
{
	Assert( index < (uint32)values.size() );
	return values[index].pName;
}

/*
==================
CSchemaEnum::GetValueAliasName
==================
*/
const char* CSchemaEnum::GetValueAliasName( uint32 index ) const
{
	Assert( index < (uint32)values.size() );
	return values[index].pAliasName;
}

/*
==================
CSchemaEnum::GetValue
==================
*/
int64 CSchemaEnum::GetValue( uint32 index ) const
{
	Assert( index < (uint32)values.size() );
	return values[index].value;
}
