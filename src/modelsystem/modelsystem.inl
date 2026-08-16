#pragma once

/*
==================
CModelSystem::AddPendingUpdateModel
==================
*/
FORCEINLINE void CModelSystem::AddPendingUpdateModel( CModel* pModel )
{
	PROFILER_SCOPE_FUNC();
	pendingUpdateModels.emplace_back( pModel );
}

/*
==================
CModelSystem::RemovePendingUpdateModel
==================
*/
FORCEINLINE void CModelSystem::RemovePendingUpdateModel( CModel* pModel )
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)pendingUpdateModels.size(); index < count; ++index )
	{
		if ( pendingUpdateModels[index] == pModel )
		{
			pendingUpdateModels.erase( pendingUpdateModels.begin() + index );
			return;
		}
	}
}
