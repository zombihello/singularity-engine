#pragma once
#include <vector>
#include <string>

#include "filesystem/ifilesystem.h"

//-----------------------------------------------------------------------------
// This is simple array who contains path elements
//-----------------------------------------------------------------------------
class CPathArrayResult : public TRefCounted<IPathArrayResult>
{
public:
    CPathArrayResult( const std::vector<std::string>& pathArray );

    // IPathArrayResult interface
    virtual const achar* GetPath( uint32 index ) const override;
    virtual uint32 GetNum() const override;
    virtual bool IsEmpty() const override;

private:
	std::vector<std::string>	pathArray;
};