#pragma once
#include "schemasystem/ischematype.h"

//-----------------------------------------------------------------------------
// A described enum
//-----------------------------------------------------------------------------
class ISchemaEnum : public ISchemaType
{
public:
	// Looks a value up by its name or by its alias name
	virtual bool		FindValueByName( const char* pName, int64& value ) const = 0;
	virtual const char* FindNameByValue( int64 value ) const					 = 0;

	virtual schemaFieldType_t GetUnderlyingType() const				  = 0;	// One of `SCHEMA_FIELD_TYPE_INT8`..`SCHEMA_FIELD_TYPE_UINT64`
	virtual uint32			  GetNumValues() const					  = 0;
	virtual const char*		  GetValueName( uint32 index ) const	  = 0;
	virtual const char*		  GetValueAliasName( uint32 index ) const = 0;
	virtual int64			  GetValue( uint32 index ) const		  = 0;
};
