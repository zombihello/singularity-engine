#include "pch_filesystem.h"
#include "filesystem/patharrayresult.h"

/*
==================
CPathArrayResult::CPathArrayResult
==================
*/
CPathArrayResult::CPathArrayResult( const eastl::vector<eastl::string>& pathArray )
	: pathArray( pathArray )
{
}

/*
==================
CPathArrayResult::GetItem
==================
*/
const char* CPathArrayResult::GetPath( uint32 index ) const
{
	Assert( index >= 0 && index < pathArray.size() );
	return pathArray[index].c_str();
}

/*
==================
CPathArrayResult::GetNum
==================
*/
uint32 CPathArrayResult::GetNum() const
{
	return (uint32)pathArray.size();
}

/*
==================
CPathArrayResult::IsEmpty
==================
*/
bool CPathArrayResult::IsEmpty() const
{
	return pathArray.empty();
}
