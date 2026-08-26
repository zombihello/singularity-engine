#pragma once
#include "schemasystem/ischemaenum.h"
#include "schemasystem/schematypebase.h"

//-----------------------------------------------------------------------------
// A described enum
//-----------------------------------------------------------------------------
class CSchemaEnum : public CSchemaTypeBase<ISchemaEnum>
{
public:
	// ISchemaEnum interface
	virtual bool		FindValueByName( const char* pName, int64& value ) const override;
	virtual const char* FindNameByValue( int64 value ) const override;

	virtual schemaFieldType_t GetUnderlyingType() const override;
	virtual uint32			  GetNumValues() const override;
	virtual const char*		  GetValueName( uint32 index ) const override;
	virtual const char*		  GetValueAliasName( uint32 index ) const override;
	virtual int64			  GetValue( uint32 index ) const override;

	CSchemaEnum( const schemaEnumDesc_t& enumDesc );

private:
	using valueDict_t = eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t>;
	using nameDict_t  = eastl::unordered_map<int64, uint32>;

	schemaFieldType_t					 underlyingType;
	eastl::vector<schemaEnumValueDesc_t> values;
	valueDict_t							 valueDict;
	nameDict_t							 nameDict;
};
