#ifndef ECSFIELDSTUBS_H
#define ECSFIELDSTUBS_H

#include <vector>

#include "stdlib/refcount.h"
#include "parserlib/filecontext.h"

//-----------------------------------------------------------------------------
// ECS stubs
//-----------------------------------------------------------------------------
enum ecsFieldAccessType_t
{
	ECS_FIELD_ACCESS_TYPE_READ,
	ECS_FIELD_ACCESS_TYPE_WRITE,
	ECS_FIELD_NUM_ACCESS_TYPES
};


enum ecsSystemStage_t
{
	ECS_SYSTEM_STAGE_ONSTART,
	ECS_SYSTEM_STAGE_ONLOAD,
	ECS_SYSTEM_STAGE_POSTLOAD,
	ECS_SYSTEM_STAGE_PREUPDATE,
	ECS_SYSTEM_STAGE_ONUPDATE,
	ECS_SYSTEM_STAGE_ONVALIDATE,
	ECS_SYSTEM_STAGE_POSTUPDATE,
	ECS_SYSTEM_STAGE_PRESTORE,
	ECS_SYSTEM_STAGE_ONSTORE
};


enum ecsSystemFilterType_t
{
	ECS_SYSTEM_FILTER_TYPE_INCLUDE,
	ECS_SYSTEM_FILTER_TYPE_EXCLUDE,
	ECS_SYSTEM_NUM_FILTER_TYPES
};


struct ecsScopeStub_t
{
	parserFileContext_t		startContext;
	parserFileContext_t		endContext;
};


class CEcsStubBase : public TRefCounted<IRefCounted>
{
public:
	CEcsStubBase( const parserFileContext_t& context, const achar* pName );

	FORCEINLINE const parserFileContext_t& GetContext() const		{ return context; }
	FORCEINLINE const achar* GetName() const						{ return name.c_str(); }

protected:
	parserFileContext_t		context;
	std::string				name;
};


class CEcsStubMetadataValue : public CEcsStubBase
{
public:
	CEcsStubMetadataValue( const parserFileContext_t& context, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );

	FORCEINLINE bool HasValue() const									{ return pValueContext; }
	FORCEINLINE const parserFileContext_t* GetValueContext() const		{ return pValueContext; }
	FORCEINLINE const achar* GetValue() const							{ return value.c_str(); }

private:
	const parserFileContext_t*	pValueContext;
	std::string					value;
};


class CEcsStubMetadata : public CEcsStubBase
{
public:
	CEcsStubMetadata( const parserFileContext_t& context );

	FORCEINLINE void AddValue( CEcsStubMetadataValue* pValue )							{ metadataValues.emplace_back( pValue ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubMetadataValue>>& GetValue() const		{ return metadataValues; }
	FORCEINLINE ecsScopeStub_t& GetScope()												{ return scope; }

private:
	ecsScopeStub_t									scope;
	std::vector<TRefPtr<CEcsStubMetadataValue>>		metadataValues;
};


class CEcsStubDefaultFieldValue : public CEcsStubBase
{
public:
	CEcsStubDefaultFieldValue( const parserFileContext_t& context, const parserFileContext_t& valueContext, const achar* pName, const achar* pValue );

	FORCEINLINE const parserFileContext_t& GetValueContext() const		{ return valueContext; }
	FORCEINLINE const achar* GetValue() const							{ return value.c_str(); }

private:
	parserFileContext_t		valueContext;
	std::string				value;
};


class CEcsStubField : public CEcsStubBase
{
public:
	CEcsStubField( const parserFileContext_t& context, const parserFileContext_t& typeContext, const achar* pName, const achar* pType, CEcsStubMetadata* pMetadata = NULL );

	FORCEINLINE const parserFileContext_t& GetTypeContext() const		{ return typeContext; }
	FORCEINLINE const achar* GetType() const							{ return type.c_str(); }
	FORCEINLINE CEcsStubMetadata* GetMetadata() const					{ return pMetadata; }

private:
	parserFileContext_t			typeContext;
	std::string					type;
	TRefPtr<CEcsStubMetadata>	pMetadata;
};


class CEcsStubComponent : public CEcsStubBase
{
public:
	CEcsStubComponent( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata = NULL );

	FORCEINLINE void AddDefaultFieldValue( CEcsStubDefaultFieldValue* pDefaultFieldValue )				{ defaultFieldValues.emplace_back( pDefaultFieldValue ); }
	FORCEINLINE void AddField( CEcsStubField* pField )													{ fields.emplace_back( pField ); }

	FORCEINLINE const std::vector<TRefPtr<CEcsStubDefaultFieldValue>>& GetDefaultFieldValues() const	{ return defaultFieldValues; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields() const							{ return fields; }
	FORCEINLINE CEcsStubMetadata* GetMetadata() const													{ return pMetadata; }
	FORCEINLINE ecsScopeStub_t& GetScope()																{ return scope; }

private:
	ecsScopeStub_t										scope;
	TRefPtr<CEcsStubMetadata>							pMetadata;
	std::vector<TRefPtr<CEcsStubDefaultFieldValue>>		defaultFieldValues;
	std::vector<TRefPtr<CEcsStubField>>					fields;
};


class CEcsStubUsing : public CEcsStubBase
{
public:
	CEcsStubUsing( const parserFileContext_t& context, const achar* pName );
};


class CEcsStubSystemFilter : public CEcsStubBase
{
public:
	CEcsStubSystemFilter( const parserFileContext_t& context, const achar* pName );
};


class CEcsStubSystem : public CEcsStubBase
{
public:
	CEcsStubSystem( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata = NULL );

	FORCEINLINE void SetStage( const parserFileContext_t& stageContext, ecsSystemStage_t stage )							
	{ 
		CEcsStubSystem::stageContext	= stageContext; 
		CEcsStubSystem::stage			= stage; 
	}
	FORCEINLINE void AddField( ecsFieldAccessType_t accessType, CEcsStubField* pField )										{ fields[accessType].emplace_back( pField ); }
	FORCEINLINE void AddFilter( ecsSystemFilterType_t filterType, CEcsStubSystemFilter* pFilter )							{ filters[filterType].emplace_back( pFilter ); }

	FORCEINLINE bool HasFields( ecsFieldAccessType_t accessType ) const														{ return !fields[accessType].empty(); }
	FORCEINLINE bool HasFilters( ecsSystemFilterType_t filterType ) const													{ return !filters[filterType].empty(); }
	FORCEINLINE ecsScopeStub_t& GetScope()																					{ return scope; }
	FORCEINLINE ecsSystemStage_t GetStage() const																			{ return stage; }
	FORCEINLINE const parserFileContext_t& GetStageContext() const															{ return stageContext; }
	FORCEINLINE CEcsStubMetadata* GetMetadata() const																		{ return pMetadata; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields( ecsFieldAccessType_t accessType ) const				{ return fields[accessType]; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubSystemFilter>>& GetFilters( ecsSystemFilterType_t filterType ) const		{ return filters[filterType]; }

private:
	ecsScopeStub_t								scope;
	ecsSystemStage_t							stage;
	parserFileContext_t							stageContext;
	TRefPtr<CEcsStubMetadata>					pMetadata;
	std::vector<TRefPtr<CEcsStubField>>			fields[ECS_FIELD_NUM_ACCESS_TYPES];
	std::vector<TRefPtr<CEcsStubSystemFilter>>	filters[ECS_SYSTEM_NUM_FILTER_TYPES];
};


class CEcsStubModule : public CEcsStubBase
{
public:
	CEcsStubModule( const parserFileContext_t& context, const achar* pName );

	FORCEINLINE void AddUsing( CEcsStubUsing* pUsing )									{ usings.emplace_back( pUsing ); }
	FORCEINLINE void AddComponent( CEcsStubComponent* pComponent )						{ components.emplace_back( pComponent ); }
	FORCEINLINE void AddSystem( CEcsStubSystem* pSystem )								{ systems.emplace_back( pSystem ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubUsing>>& GetUsings() const			{ return usings; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubComponent>>& GetComponents() const	{ return components; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubSystem>>& GetSystems() const			{ return systems; }
	FORCEINLINE ecsScopeStub_t& GetScope()												{ return scope; }

private:
	ecsScopeStub_t								scope;
	std::vector<TRefPtr<CEcsStubUsing>>			usings;
	std::vector<TRefPtr<CEcsStubComponent>>		components;
	std::vector<TRefPtr<CEcsStubSystem>>		systems;
};


class CEcsSystemStub
{
public:
	FORCEINLINE void AddModule( CEcsStubModule* pModule )						{ modules.emplace_back( pModule ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubModule>>& GetModules() const	{ return modules; }

private:
	std::vector<TRefPtr<CEcsStubModule>>		modules;
};

#endif // !ECSFIELDSTUBS_H