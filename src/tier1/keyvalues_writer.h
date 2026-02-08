#pragma once
#include <EASTL/string.h>
#include "tier1/keyvalues.h"

//-----------------------------------------------------------------------------
// Key values writer
//-----------------------------------------------------------------------------
class CKeyValuesWriter
{
public:
	void Write( CKeyValues* pKeyValues, eastl::string& buffer ) const;

private:
	void WriteKeyToBuffer( CKeyValues* pKeyValues, eastl::string& buffer, uint32 indentLevel ) const;
	void WriteIndents( eastl::string& buffer, uint32 indentLevel ) const;
	void WriteConvertedString( eastl::string& buffer, const char* pValue ) const;
};

#include "tier1/keyvalues_writer.inl"
