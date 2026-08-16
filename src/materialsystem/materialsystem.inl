#pragma once

/*
==================
CMaterialSystem::AddPendingUpdateMaterial
==================
*/
FORCEINLINE void CMaterialSystem::AddPendingUpdateMaterial( CMaterial* pMaterial )
{
	PROFILER_SCOPE_FUNC();
	pendingUpdateMaterials.emplace_back( pMaterial );
}

/*
==================
CMaterialSystem::RemovePendingUpdateMaterial
==================
*/
FORCEINLINE void CMaterialSystem::RemovePendingUpdateMaterial( CMaterial* pMaterial )
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)pendingUpdateMaterials.size(); index < count; ++index )
	{
		if ( pendingUpdateMaterials[index] == pMaterial )
		{
			pendingUpdateMaterials.erase( pendingUpdateMaterials.begin() + index );
			return;
		}
	}
}
