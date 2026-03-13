#include "pch_tier1.h"
#include "tier1/stringid.h"

/*
==================
CStringID::GetStringPool
==================
*/
CStringPool<CStringID::id_t>& CStringID::GetStringPool()
{
	static CStringPool<id_t> s_stringPool;
	return s_stringPool;
}
