#ifndef ECS_CORE_INL
#define ECS_CORE_INL

/*
==================
ecsEntity_t::operator=
==================
*/
FORCEINLINE ecsEntity_t& ecsEntity_t::operator=( const ecsEntity_t& other )
{
	flecsEntity = other.flecsEntity;
	return *this;
}


/*
==================
CEcsWorld::RegisterModule
==================
*/
template<typename TEcsModule>
void CEcsWorld::RegisterModule()
{
	flecsWorld.import<TEcsModule>();
}

/*
==================
CEcsWorld::UnRegisterModule
==================
*/
template<typename TEcsModule>
void CEcsWorld::UnRegisterModule()
{
	flecsWorld.module<TEcsModule>().destruct();
}

/*
==================
CEcsWorld::CreateEntity
==================
*/
FORCEINLINE ecsEntity_t CEcsWorld::CreateEntity( const achar* pName, ecsEntity_t ecsPrefab )
{
	AssertMsg( IsValidEntity( ecsPrefab ) && IsPrefab( ecsPrefab ), "Prefab isn't valid" );
	return ecsEntity_t( flecsWorld.entity( pName ).is_a( ecsPrefab.flecsEntity ) );
}

/*
==================
CEcsWorld::CreatePrefab
==================
*/
FORCEINLINE ecsEntity_t CEcsWorld::CreatePrefab( const achar* pName )
{
	return ecsEntity_t( flecsWorld.prefab( pName ) );
}

/*
==================
CEcsWorld::DestroyEntity
==================
*/
FORCEINLINE void CEcsWorld::DestroyEntity( ecsEntity_t& ecsEntity )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	ecsEntity.flecsEntity.destruct();
	ecsEntity.flecsEntity = flecs::entity::null();
}

/*
==================
CEcsWorld::CloneEntity
==================
*/
FORCEINLINE ecsEntity_t CEcsWorld::CloneEntity( ecsEntity_t ecsEntity, const achar* pName /* = NULL */ )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	ecsEntity_t		newEntity = ecsEntity.flecsEntity.clone();
	if ( pName && S_Strlen( pName ) > 0 )
	{
		newEntity.flecsEntity.set_name( pName );
	}
	return newEntity;
}

/*
==================
CEcsWorld::SetEntityName
==================
*/
FORCEINLINE void CEcsWorld::SetEntityName( ecsEntity_t ecsEntity, const achar* pName )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	ecsEntity.flecsEntity.set_name( pName );
}

/*
==================
CEcsWorld::GetEntityName
==================
*/
FORCEINLINE const achar* CEcsWorld::GetEntityName( const ecsEntity_t& ecsEntity ) const
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	return ecsEntity.flecsEntity.name().c_str();
}

/*
==================
CEcsWorld::IsValidEntity
==================
*/
FORCEINLINE bool CEcsWorld::IsValidEntity( const ecsEntity_t& ecsEntity ) const
{
	return flecsWorld.is_valid( ecsEntity.flecsEntity );
}

/*
==================
CEcsWorld::IsArchetypeEntity
==================
*/
FORCEINLINE bool CEcsWorld::IsPrefab( const ecsEntity_t& ecsEntity ) const
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	return ecsEntity.flecsEntity.has( flecs::Prefab );
}

/*
==================
CEcsWorld::AddResource
==================
*/
template<typename TEcsResource>
FORCEINLINE void CEcsWorld::AddResource()
{
	AssertMsg( !HasResource<TEcsResource>(), "The world already has resource" );
	flecsWorld.add<TEcsResource>();
}

/*
==================
CEcsWorld::SetResource
==================
*/
template<typename TEcsResource>
FORCEINLINE void CEcsWorld::SetResource( const TEcsResource& ecsResource )
{
	AssertMsg( !flecs::is_empty<TEcsResource>::value, "To set a resource the one must have at least one field" );
	flecsWorld.set<TEcsResource>( ecsResource );
}

/*
==================
CEcsWorld::SetResource
==================
*/
template<typename TEcsResource>
FORCEINLINE void CEcsWorld::SetResource( TEcsResource&& ecsResource )
{
	AssertMsg( !flecs::is_empty<TEcsResource>::value, "To set a resource the one must have at least one field" );
	flecsWorld.set<TEcsResource>( std::forward<TEcsResource>( ecsResource ) );
}

/*
==================
CEcsWorld::RemoveResource
==================
*/
template<typename TEcsResource>
FORCEINLINE void CEcsWorld::RemoveResource()
{
	AssertMsg( HasResource<TEcsResource>(), "The world doesn't have component" );
	flecsWorld.remove<TEcsResource>();
}

/*
==================
CEcsWorld::GetResource
==================
*/
template<typename TEcsResource>
FORCEINLINE const TEcsResource* CEcsWorld::GetResource() const
{
	AssertMsg( HasResource<TEcsResource>(), "The world doesn't have resource" );
	AssertMsg( !flecs::is_empty<TEcsResource>::value, "To get pointer to a resource, the one must have at least one field" );
	return flecsWorld.get<TEcsResource>();
}

/*
==================
CEcsWorld::GetResource
==================
*/
template<typename TEcsResource>
FORCEINLINE TEcsResource* CEcsWorld::GetResource()
{
	AssertMsg( HasResource<TEcsResource>(), "The world doesn't have resource" );
	AssertMsg( !flecs::is_empty<TEcsResource>::value, "To get pointer to a resource, the one must have at least one field" );
	return flecsWorld.get_mut<TEcsResource>();
}

/*
==================
CEcsWorld::HasResource
==================
*/
template<typename TEcsResource>
FORCEINLINE bool CEcsWorld::HasResource() const
{
	return flecsWorld.has<TEcsResource>();
}

/*
==================
CEcsWorld::AddComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::AddComponent( ecsEntity_t& ecsEntity )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( !HasComponent<TEcsComponent>( ecsEntity ), "Entity already has component" );
	ecsEntity.flecsEntity.add<TEcsComponent>();
}

/*
==================
CEcsWorld::SetComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::SetComponent( ecsEntity_t& ecsEntity, const TEcsComponent& ecsComponent )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( !flecs::is_empty<TEcsComponent>::value, "To set a component the one must have at least one field" );
	ecsEntity.flecsEntity.set<TEcsComponent>( ecsComponent );
}

/*
==================
CEcsWorld::SetComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::SetComponent( ecsEntity_t& ecsEntity, TEcsComponent&& ecsComponent )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( !flecs::is_empty<TEcsComponent>::value, "To set a component the one must have at least one field" );
	ecsEntity.flecsEntity.set<TEcsComponent>( std::forward<TEcsComponent>( ecsComponent ) );
}

/*
==================
CEcsWorld::RemoveComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::RemoveComponent( ecsEntity_t& ecsEntity )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( HasComponent<TEcsComponent>( ecsEntity ), "Entity doesn't have component" );
	ecsEntity.flecsEntity.remove<TEcsComponent>();
}

/*
==================
CEcsWorld::RemoveAllComponents
==================
*/
FORCEINLINE void CEcsWorld::RemoveAllComponents( ecsEntity_t& ecsEntity )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	ecsEntity.flecsEntity.clear();
}

/*
==================
CEcsWorld::GetComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE const TEcsComponent* CEcsWorld::GetComponent( const ecsEntity_t& ecsEntity ) const
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( HasComponent<TEcsComponent>( ecsEntity ), "Entity doesn't have component" );
	AssertMsg( !flecs::is_empty<TEcsComponent>::value, "To get pointer to a component, the one must have at least one field" );
	return ecsEntity.flecsEntity.get<TEcsComponent>();
}

/*
==================
CEcsWorld::GetComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE TEcsComponent* CEcsWorld::GetComponent( ecsEntity_t& ecsEntity )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( HasComponent<TEcsComponent>( ecsEntity ), "Entity doesn't have component" );
	AssertMsg( !flecs::is_empty<TEcsComponent>::value, "To get pointer to a component, the one must have at least one field" );
	return ecsEntity.flecsEntity.get_mut<TEcsComponent>();
}

/*
==================
CEcsWorld::HasComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE bool CEcsWorld::HasComponent( const ecsEntity_t& ecsEntity ) const
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	return ecsEntity.flecsEntity.has<TEcsComponent>();
}

/*
==================
CEcsWorld::Update
==================
*/
FORCEINLINE void CEcsWorld::Update( float deltaTime )
{
	flecsWorld.progress( deltaTime );
}

#endif // !ECS_INL