#ifndef ECSFIELDSTUBS_H
#define ECSFIELDSTUBS_H

#include <vector>

#include "stdlib/refcount.h"
#include "parserlib/filecontext.h"

//-----------------------------------------------------------------------------
// ECS stubs
//-----------------------------------------------------------------------------
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
	CEcsStubField( const parserFileContext_t& context, const parserFileContext_t& typeContext, const achar* pName, const achar* pType );

	FORCEINLINE const parserFileContext_t& GetTypeContext() const		{ return typeContext; }
	FORCEINLINE const achar* GetType() const							{ return type.c_str(); }

private:
	parserFileContext_t		typeContext;
	std::string				type;
};


class CEcsStubComponent : public CEcsStubBase
{
public:
	CEcsStubComponent( const parserFileContext_t& context, const achar* pName );

	FORCEINLINE void AddDefaultFieldValue( CEcsStubDefaultFieldValue* pDefaultFieldValue )				{ defaultFieldValues.emplace_back( pDefaultFieldValue ); }
	FORCEINLINE void AddField( CEcsStubField* pField )													{ fields.emplace_back( pField ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubDefaultFieldValue>>& GetDefaultFieldValues() const	{ return defaultFieldValues; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields() const							{ return fields; }
	FORCEINLINE ecsScopeStub_t& GetScope()																{ return scope; }

private:
	ecsScopeStub_t										scope;
	std::vector<TRefPtr<CEcsStubDefaultFieldValue>>		defaultFieldValues;
	std::vector<TRefPtr<CEcsStubField>>					fields;
};


class CEcsStubUsing : public CEcsStubBase
{
public:
	CEcsStubUsing( const parserFileContext_t& context, const achar* pName );
};


class CEcsStubModule : public CEcsStubBase
{
public:
	CEcsStubModule( const parserFileContext_t& context, const achar* pName );

	FORCEINLINE void AddUsing( CEcsStubUsing* pUsing )									{ usings.emplace_back( pUsing ); }
	FORCEINLINE void AddComponent( CEcsStubComponent* pComponent )						{ components.emplace_back( pComponent ); }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubUsing>>& GetUsings() const			{ return usings; }
	FORCEINLINE const std::vector<TRefPtr<CEcsStubComponent>>& GetComponents() const	{ return components; }
	FORCEINLINE ecsScopeStub_t& GetScope()												{ return scope; }

private:
	ecsScopeStub_t								scope;
	std::vector<TRefPtr<CEcsStubUsing>>			usings;
	std::vector<TRefPtr<CEcsStubComponent>>		components;
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