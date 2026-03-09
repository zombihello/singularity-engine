#pragma once

/*
==================
TEcsComponentFactory::TEcsComponentFactory
==================
*/
template<typename TEcsComponent, void ( *pParserSENTComponentVarsFunc )( const CSENTEntityDescComponent&, TEcsComponent& )>
TEcsComponentFactory<TEcsComponent, pParserSENTComponentVarsFunc>::TEcsComponentFactory( const CSENTEntityDescComponent& sentComponent )
{
	if ( pParserSENTComponentVarsFunc )
	{
		pParserSENTComponentVarsFunc( sentComponent, ecsArchetypeComponent );
	}
}

/*
==================
TEcsComponentFactory::TEcsComponentFactory
==================
*/
template<typename TEcsComponent, void ( *pParserSENTComponentVarsFunc )( const CSENTEntityDescComponent&, TEcsComponent& )>
TEcsComponentFactory<TEcsComponent, pParserSENTComponentVarsFunc>::TEcsComponentFactory( const TEcsComponentFactory& other )
	: ecsArchetypeComponent( other.ecsArchetypeComponent )
{
}

/*
==================
TEcsComponentFactory::Create
==================
*/
template<typename TEcsComponent, void ( *pParserSENTComponentVarsFunc )( const CSENTEntityDescComponent&, TEcsComponent& )>
void TEcsComponentFactory<TEcsComponent, pParserSENTComponentVarsFunc>::Create( CEcsWorld ecsWorld, ecsEntity_t ecsEntity )
{
	PROFILER_SCOPE_FUNC()
	if ( !ecsWorld.IsRegisteredType<TEcsComponent>() )
	{
		EnsureMsg( false, "ECS component type '%s' isn't registered in the world", TEcsComponent::GetComponentName() );
		return;
	}

	if ( !flecs::is_empty<TEcsComponent>::value )
	{
		ecsWorld.SetComponent( ecsEntity, ecsArchetypeComponent );
	}
	else
	{
		ecsWorld.AddComponent<TEcsComponent>( ecsEntity );
	}
}

/*
==================
CEcsComponentTypes::RegisterType
==================
*/
FORCEINLINE void CEcsComponentTypes::RegisterType( const char* pTypeName, createEcsComponentFactoryFn_t pCreateFunc )
{
	// Make sure that we haven't any component by the type name and pCreateFunc is valid
	PROFILER_SCOPE_FUNC()
	AssertMsg( componentTypesDict.find( pTypeName ) == componentTypesDict.end(), "You can't register more one component by same type name" );
	AssertMsg( pCreateFunc, "Invalid a create function" );

	componentTypesDict[pTypeName] = pCreateFunc;
	Msg( "Game: ECS component type '%s' registered", pTypeName );
}

/*
==================
CEcsComponentTypes::UnregisterType
==================
*/
FORCEINLINE void CEcsComponentTypes::UnregisterType( const char* pTypeName )
{
	PROFILER_SCOPE_FUNC()
	componentTypesDict.erase( pTypeName );
	Msg( "Game: ECS component type '%s' unregistered", pTypeName );
}

/*
==================
CEcsComponentTypes::UnregisterAllTypes
==================
*/
FORCEINLINE void CEcsComponentTypes::UnregisterAllTypes()
{
	PROFILER_SCOPE_FUNC()
	componentTypesDict.clear();
	Msg( "Game: All ECS component types are unregistered" );
}

/*
==================
CEcsComponentTypes::CreateFactory
==================
*/
FORCEINLINE IEcsComponentFactory* CEcsComponentTypes::CreateFactory( const CSENTEntityDescComponent& sentComponent ) const
{
	PROFILER_SCOPE_FUNC()
	const char* pTypeName = sentComponent.GetType();
	auto		 itFind	   = componentTypesDict.find( pTypeName );
	if ( itFind == componentTypesDict.end() )
	{
		Warning( "Game: Unknown ECS component type '%s'", pTypeName );
		return NULL;
	}

	return itFind->second( sentComponent );
}
