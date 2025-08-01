#pragma once

/*
==================
ResourceSystem_IsValidClassForType
==================
*/
template<>
FORCEINLINE bool ResourceSystem_IsValidClassForType<CEcsMap>( resourceType_t type )
{
	return type == RESOURCE_TYPE_MAP;
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