#pragma once
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

#include "tier1/refcount.h"
#include "tier1/stl.h"
#include "utils/sentdoc/sent_compiled_doc.h"
#include "game/shared/ecs/ecs_core.h"

//-----------------------------------------------------------------------------
// ECS component factory
// The factory creates a component and add it to an entity
//-----------------------------------------------------------------------------
class IEcsComponentFactory : public IRefCounted
{
public:
	virtual ~IEcsComponentFactory() {}
	virtual void Create( CEcsWorld ecsWorld, ecsEntity_t ecsEntity ) = 0;
};

template<typename TEcsComponent, void ( *pParserSENTComponentVarsFunc )( const CSENTEntityDescComponent& /*sentComponent*/, TEcsComponent& /*ecsComponent*/ )>
class CEcsComponentFactory : public CRefCounted<IEcsComponentFactory>
{
public:
	// IEcsComponentFactory interface
	virtual void Create( CEcsWorld ecsWorld, ecsEntity_t ecsEntity ) override;

	CEcsComponentFactory( const CSENTEntityDescComponent& sentComponent );
	CEcsComponentFactory( const CEcsComponentFactory& other );

private:
	TEcsComponent ecsArchetypeComponent;
};

//-----------------------------------------------------------------------------
// ECS component types
//-----------------------------------------------------------------------------
typedef IEcsComponentFactory* ( *createEcsComponentFactoryFn_t )( const CSENTEntityDescComponent& /*sentComponent*/ );

class CEcsComponentTypes
{
public:
	void				  RegisterType( const char* pTypeName, createEcsComponentFactoryFn_t pCreateFunc );
	void				  UnregisterType( const char* pTypeName );
	void				  UnregisterAllTypes();
	IEcsComponentFactory* CreateFactory( const CSENTEntityDescComponent& sentComponent ) const;

private:
	typedef eastl::unordered_map<const char*, createEcsComponentFactoryFn_t, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> componentTypesDict_t;
	componentTypesDict_t																												componentTypesDict;
};

#include "game/shared/ecs/ecs_component_factory.inl"
