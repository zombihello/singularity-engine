#pragma once

//-----------------------------------------------------------------------------
// ECS macroses
//-----------------------------------------------------------------------------
#define ECS_COMPONENT_BODY( EcsComponentName ) \
	static const achar* GetComponentName() \
	{ \
		return EcsComponentName; \
	}

#define ECS_RESOURCE_BODY( EcsResourceName ) \
	static const achar* GetResourceName() \
	{ \
		return EcsResourceName; \
	}

#define ECS_SYSTEM_BODY( EcsSystemName ) \
	static const achar* GetSystemName() \
	{ \
		return EcsSystemName; \
	}


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CEcsWorld;


//-----------------------------------------------------------------------------
// ECS initialize a world
//-----------------------------------------------------------------------------
void EcsInitWorld_GameframeworkOnly( CEcsWorld& ecsWorld );
void EcsInitWorld_GameOnly( CEcsWorld& ecsWorld );			// NOTE: The function must be implemented on the game side
void EcsInitWorld( CEcsWorld& ecsWorld );					// The function calls EcsInitWorld_GameframeworkOnly and EcsInitWorld_GameOnly


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
	CEcsWorld( bool bAutoInit = true )
	{
		if ( bAutoInit )
		{
			EcsInitWorld( *this );
		}
	}
	CEcsWorld( const flecs::world& flecsWorld )
		: flecsWorld( flecsWorld )
	{}

	void Update( float deltaTime );
	void Reset( bool bAutoInit = true );		// Deletes and recreates the world
	template<typename TEcsType>
	bool IsRegisteredType() const;

	// Register and unregister module functions
	template<typename TEcsModule>
	void RegisterModule();
	template<typename TEcsModule>
	void UnRegisterModule();

	// Functions to work with resources (global game resources)
	template<typename TEcsResource>
	void AddResource();
	template<typename TEcsResource>
	void SetResource( const TEcsResource& ecsResource );
	template<typename TEcsResource>
	void SetResource( TEcsResource&& ecsResource );
	template<typename TEcsResource>
	void RemoveResource();
	template<typename TEcsResource>
	const TEcsResource* GetResource() const;
	template<typename TEcsResource>
	TEcsResource* GetResource();
	template<typename TEcsResource>
	bool HasResource() const;

	// Functions to work with entities and archetypes
	ecsEntity_t CreateEntity( const achar* pName, ecsEntity_t ecsPrefab );
	ecsEntity_t CreatePrefab( const achar* pName );
	void DestroyEntity( ecsEntity_t& ecsEntity );
	ecsEntity_t CloneEntity( ecsEntity_t ecsEntity, const achar* pName = NULL );
	void SetEntityName( ecsEntity_t ecsEntity, const achar* pName );
	const achar* GetEntityName( const ecsEntity_t& ecsEntity ) const;
	ecsEntity_t FindEntity( const achar* pName ) const;
	bool IsValidEntity( const ecsEntity_t& ecsEntity ) const;
	bool IsPrefab( const ecsEntity_t& ecsEntity ) const;

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

#include "gameframework/ecs/ecs_core.inl"