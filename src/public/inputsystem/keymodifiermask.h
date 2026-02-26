#pragma once
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Key modifier mask
//-----------------------------------------------------------------------------
struct keyModifierMask_t
{
	union
	{
		uint16 flags;
		struct
		{
			bool bAlt : 1;
			bool bControl : 1;
			bool bShift : 1;
			bool bSuper : 1;
			bool bCapsLock : 1;
			bool bNumLock : 1;
		};
	};
};
