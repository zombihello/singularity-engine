#pragma once

/*
==================
CCmdSystem::GetCmdList
==================
*/
FORCEINLINE const eastl::list<commandDef_t>& CCmdSystem::GetCmdList() const
{
	return cmdList;
}
