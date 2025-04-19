#ifndef ISTUDIOAPI_RESOURCE_H
#define ISTUDIOAPI_RESOURCE_H

#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// StudioAPI base resource interface
//-----------------------------------------------------------------------------
class IStudioAPIResource : public IRefCounted
{
public:
	virtual ~IStudioAPIResource() {}
};

#endif // !ISTUDIOAPI_RESOURCE_H