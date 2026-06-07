#pragma once

/*
==================
CCVarSystem::GetCVarList
==================
*/
FORCEINLINE const eastl::list<ICVar*>& CCVarSystem::GetCVarList() const
{
	return cvarList;
}
