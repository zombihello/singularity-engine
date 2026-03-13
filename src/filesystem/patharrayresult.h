#pragma once
#include <EASTL/vector.h>
#include <EASTL/string.h>

#include "filesystem/ifilesystem.h"

//-----------------------------------------------------------------------------
// This is simple array who contains path elements
//-----------------------------------------------------------------------------
class CPathArrayResult : public CRefCounted<IPathArrayResult>
{
public:
	CPathArrayResult( const eastl::vector<eastl::string>& pathArray );

	// IPathArrayResult interface
	virtual const char* GetPath( uint32 index ) const override;
	virtual uint32		GetNum() const override;
	virtual bool		IsEmpty() const override;

private:
	eastl::vector<eastl::string> pathArray;
};
