#pragma once
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// StudioAPI base resource interface
//-----------------------------------------------------------------------------
class IStudioAPIResource : public IRefCounted
{
public:
	virtual ~IStudioAPIResource() {}
};