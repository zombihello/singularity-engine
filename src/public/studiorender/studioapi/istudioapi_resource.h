#pragma once
#include "tier1/refcount.h"

//-----------------------------------------------------------------------------
// StudioAPI base resource interface
//-----------------------------------------------------------------------------
class IStudioAPIResource : public IRefCounted
{
public:
	virtual ~IStudioAPIResource() {}
};