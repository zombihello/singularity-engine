#include "pch_gameframework.h"
#include "gameframework/ecs/ecs_core.h"

/*
==================
ecsEntity_t::ecsEntity_t
==================
*/
ecsEntity_t::ecsEntity_t()
	: flecsEntity( flecs::entity::null() )
{}

/*
==================
ecsEntity_t::ecsEntity_t
==================
*/
ecsEntity_t::ecsEntity_t( const ecsEntity_t& other )
	: flecsEntity( other.flecsEntity )
{}

/*
==================
ecsEntity_t::ecsEntity_t
==================
*/
ecsEntity_t::ecsEntity_t( const flecs::entity& flecsEntity )
	: flecsEntity( flecsEntity )
{}