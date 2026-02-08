#include "pch_tier1.h"
#include "tier1/stringid.h"

/*
==================
CStringID::GetStringPool
==================
*/
TStringPool<CStringID::id_t>& CStringID::GetStringPool()
{
	static TStringPool<id_t> s_stringPool;
	return s_stringPool;
}
