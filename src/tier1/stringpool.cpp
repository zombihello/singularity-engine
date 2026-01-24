#include "pch_tier1.h"
#include "tier1/stringpool.h"

/*
==================
GetGlobalStringPool
==================
*/
TStringPool<uint16>& GetGlobalStringPool()
{
	static TStringPool<uint16> s_globalStringPool;
	return s_globalStringPool;
}
