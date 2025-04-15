#ifndef ECS_CORE_H
#define ECS_CORE_H

class CEcsWorld;

struct ecsEntity_t
{
	friend CEcsWorld;

	ecsEntity_t();
	ecsEntity_t( const ecsEntity_t& other );
	ecsEntity_t& operator=( const ecsEntity_t& other );

private:
	ecsEntity_t( const flecs::entity& flecsEntity );

	flecs::entity		flecsEntity;
};

class CEcsWorld
{
public:
	void Update( float deltaTime );
	
	template<typename TEcsModule>
	void RegisterModule();
	template<typename TEcsModule>
	void UnRegisterModule();

	ecsEntity_t CreateEntity( const achar* pName );
	void DestroyEntity( ecsEntity_t& ecsEntity );
	const achar* GetEntityName( const ecsEntity_t& ecsEntity ) const;
	bool IsValidEntity( const ecsEntity_t& ecsEntity ) const;

	template<typename TEcsComponent>
	void AddComponent( ecsEntity_t& ecsEntity, const TEcsComponent& ecsComponent );
	template<typename TEcsComponent>
	void AddComponent( ecsEntity_t& ecsEntity, TEcsComponent&& ecsComponent );
	template<typename TEcsComponent>
	void RemoveComponent( ecsEntity_t& ecsEntity );
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

#endif // !ECS_CORE_H