#pragma once

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ISchemaTypeInternal;

//-----------------------------------------------------------------------------
// Schema scope
//-----------------------------------------------------------------------------
class CSchemaScope
{
public:
	CSchemaScope( schemaScopeId_t scopeId, const char* pName );
	~CSchemaScope();

	// Resolve/unresolve/close the scope
	void Resolve();
	void Unresolve();
	void Close();
	bool IsResolved() const;

	// Registering a described type
	void RegisterEnum( const schemaEnumDesc_t& enumDesc );
	void RegisterInterface( const schemaInterfaceDesc_t& interfaceDesc );
	void RegisterStruct( const schemaStructDesc_t& structDesc );
	void RegisterClass( const schemaClassDesc_t& classDesc );

	// Finds a type by its name or by its alias name
	const ISchemaTypeInternal* FindType( const char* pName ) const;

	// Iterate over every registered type
	schemaScopeId_t			   GetId() const;
	const char*				   GetName() const;
	uint32					   GetNumTypes() const;
	const ISchemaTypeInternal* GetType( uint32 index ) const;

private:
	using typeDict_t = eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t>;

	template<class TSchemaType, typename TSchemaTypeDesc>
	void RegisterType( const TSchemaTypeDesc& typeDesc );
	void CheckHasNoDependents() const;
	void Clear();

	schemaScopeId_t						id;
	eastl::string						name;
	bool								bResolved;
	eastl::vector<ISchemaTypeInternal*> types;
	typeDict_t							typeDict;
};

#include "schemasystem/schemascope.inl"
