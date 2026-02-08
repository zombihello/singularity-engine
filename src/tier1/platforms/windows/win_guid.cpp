#include "pch_tier1.h"
#include <combaseapi.h>

#include "tier1/guid.h"

/*
==================
CGuid::Generate
==================
*/
void CGuid::Generate( CGuid& guid )
{
	HRESULT result = CoCreateGuid( (GUID*)&guid );
	Assert( result == S_OK );
}
