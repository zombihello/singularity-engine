#ifndef ECSFIELDSTUBS_H
#define ECSFIELDSTUBS_H

#include <vector>
#include <unordered_map>

#include "stdlib/refcount.h"
#include "parserlib/filecontext.h"

//-----------------------------------------------------------------------------
// ECS stubs
//-----------------------------------------------------------------------------
enum ecsMetadataType_t
{
	ECS_METADATA_TYPE_SERIALIZE,
	ECS_METADATA_TYPE_NAME,
	ECS_METADATA_TYPE_PROFILER_GROUP,
	ECS_METADATA_TYPE_STAGE,
	ECS_METADATA_NUM_TYPES
};


enum ecsFieldAccessType_t
{
	ECS_FIELD_ACCESS_TYPE_READ,
	ECS_FIELD_ACCESS_TYPE_READ_OPTIONAL,
	ECS_FIELD_ACCESS_TYPE_READ_RESOURCE,
	ECS_FIELD_ACCESS_TYPE_WRITE,
	ECS_FIELD_ACCESS_TYPE_WRITE_OPTIONAL,
	ECS_FIELD_ACCESS_TYPE_WRITE_RESOURCE,
	ECS_FIELD_NUM_ACCESS_TYPES
};


enum ecsSystemFilterType_t
{
	ECS_SYSTEM_FILTER_TYPE_INCLUDE,
	ECS_SYSTEM_FILTER_TYPE_EXCLUDE,
	ECS_SYSTEM_NUM_FILTER_TYPES
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
	ECS_SYSTEM_STAGE_ONSTORE,
	ECS_SYSTEM_NUM_STAGES
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
	CEcsStubMetadataValue( const parserFileContext_t& context, ecsMetadataType_t type );
	CEcsStubMetadataValue( const parserFileContext_t& context, const parserFileContext_t& pValueContext, ecsMetadataType_t type, const achar* pValue );

	FORCEINLINE bool HasValue() const									{ return bHasValue; }
	FORCEINLINE const parserFileContext_t& GetValueContext() const		{ return valueContext; }
	FORCEINLINE const achar* GetValue() const							{ return value.c_str(); }
	FORCEINLINE ecsMetadataType_t GetType() const						{ return type; }

private:
	bool						bHasValue;
	parserFileContext_t			valueContext;
	std::string					value;
	ecsMetadataType_t			type;
};


class CEcsStubMetadata : public CEcsStubBase
{
public:
	CEcsStubMetadata( const parserFileContext_t& context );

	FORCEINLINE void AddValue( CEcsStubMetadataValue* pValue )														{ valuesDict[pValue->GetType()] = pValue; }
	FORCEINLINE const std::unordered_map<ecsMetadataType_t, TRefPtr<CEcsStubMetadataValue>>& GetValues() const		{ return valuesDict; }
	FORCEINLINE ecsScopeStub_t& GetScope()																			{ return scope; }
	FORCEINLINE bool HasValue( ecsMetadataType_t type ) const														{ return valuesDict.find( type ) != valuesDict.end(); }
	FORCEINLINE CEcsStubMetadataValue* GetValue( ecsMetadataType_t type ) const
	{
		auto itFind = valuesDict.find( type );
		if ( itFind == valuesDict.end() )
		{
			return NULL;
		}
		return itFind->second;
	}

private:
	ecsScopeStub_t															scope;
	std::unordered_map<ecsMetadataType_t, TRefPtr<CEcsStubMetadataValue>>	valuesDict;
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


class CEcsStubDataType : public CEcsStubBase
{
public:
	CEcsStubDataType( const parserFileContext_t& context, const achar* pName, CEcsStubMetadata* pMetadata = NULL );

	FORCEINLINE void AddField( CEcsStubField* pField )													{ fields.emplace_back( pField ); }
	FORCEINLINE void AddDefaultFieldValue( CEcsStubDefaultFieldValue* pDefaultFieldValue )				
	{ 
		uint32	defaultFieldValueIdx = ( uint32 )defaultFieldValues.size();
		defaultFieldValues.emplace_back( pDefaultFieldValue );
		defaultFieldValuesDict[pDefaultFieldValue->GetName()] = defaultFieldValueIdx;
	}
	FORCEINLINE CEcsStubDefaultFieldValue* FindDefaultFieldValue( const achar* pFieldName ) const
	{
		auto itFind = defaultFieldValuesDict.find( pFieldName );
		if ( itFind == defaultFieldValuesDict.end() )
		{
			return NULL;
		}
		return defaultFieldValues[itFind->second];
	}

	FORCEINLINE const std::vector<TRefPtr<CEcsStubDefaultFieldValue>>& GetDefaultFieldValues() const	{ return defaultFieldValues; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields() const							{ return fields; }
	FORCEINLINE CEcsStubMetadata* GetMetadata() const													{ return pMetadata; }
	FORCEINLINE ecsScopeStub_t& GetScope()																{ return scope; }

private:
	ecsScopeStub_t										scope;
	TRefPtr<CEcsStubMetadata>							pMetadata;
	std::vector<TRefPtr<CEcsStubDefaultFieldValue>>		defaultFieldValues;
	std::vector<TRefPtr<CEcsStubField>>					fields;
	std::unordered_map<std::string, uint32>				defaultFieldValuesDict;
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

	FORCEINLINE void AddField( ecsFieldAccessType_t accessType, CEcsStubField* pField )										{ fields[accessType].emplace_back( pField ); }
	FORCEINLINE void AddFilter( ecsSystemFilterType_t filterType, CEcsStubSystemFilter* pFilter )							{ filters[filterType].emplace_back( pFilter ); }

	FORCEINLINE bool HasFields( ecsFieldAccessType_t accessType ) const														{ return !fields[accessType].empty(); }
	FORCEINLINE bool HasFilters( ecsSystemFilterType_t filterType ) const													{ return !filters[filterType].empty(); }
	FORCEINLINE ecsScopeStub_t& GetScope()																					{ return scope; }
	FORCEINLINE CEcsStubMetadata* GetMetadata() const																		{ return pMetadata; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields( ecsFieldAccessType_t accessType ) const				{ return fields[accessType]; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubSystemFilter>>& GetFilters( ecsSystemFilterType_t filterType ) const		{ return filters[filterType]; }

private:
	ecsScopeStub_t								scope;
	TRefPtr<CEcsStubMetadata>					pMetadata;
	std::vector<TRefPtr<CEcsStubField>>			fields[ECS_FIELD_NUM_ACCESS_TYPES];
	std::vector<TRefPtr<CEcsStubSystemFilter>>	filters[ECS_SYSTEM_NUM_FILTER_TYPES];
};


class CEcsStubModule : public CEcsStubBase
{
public:
	CEcsStubModule( const parserFileContext_t& context, const achar* pName );

	FORCEINLINE void AddUsing( CEcsStubUsing* pUsing )									{ usings.emplace_back( pUsing ); }
	FORCEINLINE void AddComponent( CEcsStubDataType* pComponent )						{ components.emplace_back( pComponent ); }
	FORCEINLINE void AddSystem( CEcsStubSystem* pSystem )								{ systems.emplace_back( pSystem ); }
	FORCEINLINE void AddResource( CEcsStubDataType* pResource )							{ resources.emplace_back( pResource ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubUsing>>& GetUsings() const			{ return usings; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubDataType>>& GetComponents() const		{ return components; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubSystem>>& GetSystems() const			{ return systems; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubDataType>>& GetResources() const		{ return resources; }
	FORCEINLINE ecsScopeStub_t& GetScope()												{ return scope; }

private:
	ecsScopeStub_t								scope;
	std::vector<TRefPtr<CEcsStubUsing>>			usings;
	std::vector<TRefPtr<CEcsStubDataType>>		components;
	std::vector<TRefPtr<CEcsStubDataType>>		resources;
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