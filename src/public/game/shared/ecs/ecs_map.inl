#pragma once

/*
==================
CEcsMap::IsInitialized
==================
*/
FORCEINLINE bool CEcsMap::IsInitialized() const
{
	return bInitialized;
}

/*
==================
CEcsMap::GetEcsWorld
==================
*/
FORCEINLINE CEcsWorld& CEcsMap::GetEcsWorld()
{
	return ecsWorld;
}

/*
==================
CEcsMap::GetStudioScene
==================
*/
FORCEINLINE IStudioScene* CEcsMap::GetStudioScene() const
{
	return pStudioScene;
}