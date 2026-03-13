#pragma once
#include <EASTL/vector.h>
#include <EASTL/list.h>

#include "appframework/iwindowmgr.h"

//-----------------------------------------------------------------------------
// A window pool
//-----------------------------------------------------------------------------
template<typename T>
class CWindowPool
{
public:
	~CWindowPool();

	T*	 Create();
	void Free( windowId_t id );
	T*	 Get( windowId_t id ) const;

private:
	eastl::vector<T*>		windows;
	eastl::list<windowId_t> freeIds;
};

#include "appframework/windowpool.inl"
