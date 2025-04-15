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
FORCEINLINE ecsEntity_t CEcsWorld::CreateEntity( const achar* pName )
{
	return ecsEntity_t( flecsWorld.entity( pName ) );
}

/*
==================
CEcsWorld::DestroyEntity
==================
*/
FORCEINLINE void CEcsWorld::DestroyEntity( ecsEntity_t& ecsEntity )
{
	ecsEntity.flecsEntity.destruct();
	ecsEntity.flecsEntity = flecs::entity::null();
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
	return ecsEntity.flecsEntity != flecs::entity::null();
}

/*
==================
CEcsWorld::AddComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::AddComponent( ecsEntity_t& ecsEntity, const TEcsComponent& ecsComponent )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( !HasComponent<TEcsComponent>( ecsEntity ), "Entity already has component" );
	ecsEntity.flecsEntity.set<TEcsComponent>( ecsComponent );
}

/*
==================
CEcsWorld::AddComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE void CEcsWorld::AddComponent( ecsEntity_t& ecsEntity, TEcsComponent&& ecsComponent )
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( !HasComponent<TEcsComponent>( ecsEntity ), "Entity already has component" );
	ecsEntity.flecsEntity.set<TEcsComponent>( std::move( ecsComponent ) );
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
CEcsWorld::GetComponent
==================
*/
template<typename TEcsComponent>
FORCEINLINE const TEcsComponent* CEcsWorld::GetComponent( const ecsEntity_t& ecsEntity ) const
{
	AssertMsg( IsValidEntity( ecsEntity ), "Entity must be valid" );
	AssertMsg( HasComponent<TEcsComponent>( ecsEntity ), "Entity doesn't have component" );
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

#endif // !ECS_CORE_INL