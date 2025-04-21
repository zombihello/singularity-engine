#ifndef ECS_H
#define ECS_H

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsWorld;


//-----------------------------------------------------------------------------
// ECS entity handle
//-----------------------------------------------------------------------------
struct ecsEntity_t
{
	friend CEcsWorld;

	ecsEntity_t()
		: flecsEntity( flecs::entity::null() )
	{}
	ecsEntity_t( const flecs::entity& flecsEntity )
		: flecsEntity( flecsEntity )
	{}
	ecsEntity_t( const ecsEntity_t& other )
		: flecsEntity( other.flecsEntity )
	{}

	ecsEntity_t& operator=( const ecsEntity_t& other );

private:
	flecs::entity		flecsEntity;
};


//-----------------------------------------------------------------------------
// ECS world
//-----------------------------------------------------------------------------
class CEcsWorld
{
public:
	CEcsWorld()
	{}
	CEcsWorld( const flecs::world& flecsWorld )
		: flecsWorld( flecsWorld )
	{}

	void Update( float deltaTime );
	// Deletes and recreates the world
	// NOTE: After reset you have to re-register modules
	void Reset();

	// Register and unregister module functions
	template<typename TEcsModule>
	void RegisterModule();
	template<typename TEcsModule>
	void UnRegisterModule();

	// Functions to work with entities
	ecsEntity_t CreateEntity( const achar* pName );
	void DestroyEntity( ecsEntity_t& ecsEntity );
	ecsEntity_t CloneEntity( ecsEntity_t ecsEntity, const achar* pName = NULL );
	void SetEntityName( ecsEntity_t ecsEntity, const achar* pName );
	const achar* GetEntityName( const ecsEntity_t& ecsEntity ) const;
	bool IsValidEntity( const ecsEntity_t& ecsEntity ) const;

	// Functions to works with components
	template<typename TEcsComponent>
	void AddComponent( ecsEntity_t& ecsEntity );
	template<typename TEcsComponent>
	void SetComponent( ecsEntity_t& ecsEntity, const TEcsComponent& ecsComponent );
	template<typename TEcsComponent>
	void SetComponent( ecsEntity_t& ecsEntity, TEcsComponent&& ecsComponent );
	template<typename TEcsComponent>
	void RemoveComponent( ecsEntity_t& ecsEntity );
	void RemoveAllComponents( ecsEntity_t& ecsEntity );
	template<typename TEcsComponent>
	const TEcsComponent* GetComponent( const ecsEntity_t& ecsEntity ) const;
	template<typename TEcsComponent>
	TEcsComponent* GetComponent( ecsEntity_t& ecsEntity );
	template<typename TEcsComponent>
	bool HasComponent( const ecsEntity_t& ecsEntity ) const;

private:
	flecs::world	flecsWorld;
};

#include "gameframework/ecs/ecs.inl"

#endif // !ECS_H