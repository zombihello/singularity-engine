#pragma once
#include <string>
#include <unordered_map>

#include "stdlib/refcount.h"
#include "stdlib/stl.h"
#include "sentdoc/sent_compiled_doc.h"
#include "gameframework/ecs/ecs_core.h"

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
class TEcsComponentFactory : public TRefCounted<IEcsComponentFactory>
{
public:
	// IEcsComponentFactory interface
	virtual void Create( CEcsWorld ecsWorld, ecsEntity_t ecsEntity ) override;

	TEcsComponentFactory( const CSENTEntityDescComponent& sentComponent );
	TEcsComponentFactory( const TEcsComponentFactory& other );

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
	typedef std::unordered_map<const char*, createEcsComponentFactoryFn_t, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> componentTypesDict_t;
	componentTypesDict_t																											   componentTypesDict;
};

#include "gameframework/ecs/ecs_component_factory.inl"