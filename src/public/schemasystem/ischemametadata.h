#pragma once
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// Metadata attached to a described type or field
//-----------------------------------------------------------------------------
class ISchemaMetadata
{
public:
	virtual const char* FindValue( const char* pKey, const char* pDefaultValue = "" ) const = 0;

	virtual uint32		GetNum() const				   = 0;
	virtual const char* GetKey( uint32 index ) const   = 0;
	virtual const char* GetValue( uint32 index ) const = 0;
};
