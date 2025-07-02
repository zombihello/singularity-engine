#ifndef ECS_MAP_INL
#define ECS_MAP_INL

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

#endif // !ECS_MAP_INL