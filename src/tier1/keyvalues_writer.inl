#pragma once

/*
==================
CKeyValuesWriter::WriteIndents
==================
*/
FORCEINLINE void CKeyValuesWriter::WriteIndents( eastl::string& buffer, uint32 indentLevel ) const
{
	for ( uint32 index = 0; index < indentLevel; ++index )
	{
		buffer += '\t';
	}
}
