#pragma once
#include "appframework/iappsystem.h"
#include "schemasystem/ischemasystem.h"
#include "schemasystem/schemascope.h"

//-----------------------------------------------------------------------------
// The schema system
//-----------------------------------------------------------------------------
class CSchemaSystem : public CBaseAppSystem<ISchemaSystem>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;
	virtual bool Init() override;
	virtual void Shutdown() override;

	// ISchemaSystem interface
	virtual schemaScopeId_t OpenScope( const char* pName ) override;
	virtual void			CloseScope( schemaScopeId_t scopeId ) override;

	virtual void RegisterEnum( const schemaEnumDesc_t& enumDesc, schemaScopeId_t scopeId ) override;
	virtual void RegisterInterface( const schemaInterfaceDesc_t& interfaceDesc, schemaScopeId_t scopeId ) override;
	virtual void RegisterStruct( const schemaStructDesc_t& structDesc, schemaScopeId_t scopeId ) override;
	virtual void RegisterClass( const schemaClassDesc_t& classDesc, schemaScopeId_t scopeId ) override;

	virtual const ISchemaType* FindType( const char* pName ) const override;

	virtual uint32			   GetNumScopes() const override;
	virtual schemaScope_t	   GetScope( uint32 index ) const override;
	virtual uint32			   GetNumTypes( schemaScopeId_t scopeId ) const override;
	virtual const ISchemaType* GetType( schemaScopeId_t scopeId, uint32 index ) const override;
	virtual uint32			   GetGeneration() const override;

	CSchemaSystem();

	ISchemaTypeInternal* FindTypeInternal( const char* pName ) const;
	CSchemaScope*		 GetScopeById( schemaScopeId_t scopeId ) const;
	const char*			 GetScopeNameById( schemaScopeId_t scopeId ) const;

private:
	enum
	{
		SCHEMA_SCOPE_INDEX_BITS = 16,
		SCHEMA_SCOPE_INDEX_MASK = ( 1 << SCHEMA_SCOPE_INDEX_BITS ) - 1,
		SCHEMA_SCOPE_MAX_SCOPES = SCHEMA_SCOPE_INDEX_MASK
	};

	struct scopeSlot_t
	{
		CSchemaScope* pScope;
		uint16		  generation;
	};

	static schemaScopeId_t MakeScopeId( uint16 index, uint16 generation );
	static uint16		   GetScopeIndex( schemaScopeId_t scopeId );
	static uint16		   GetScopeGeneration( schemaScopeId_t scopeId );

	bool					   bInited;
	uint32					   generation;
	eastl::vector<scopeSlot_t> scopes;
	eastl::vector<uint16>	   openScopeIndices;
	eastl::list<uint16>		   freeScopeIndexList;
};

extern CSchemaSystem g_schemaSystem;
#include "schemasystem/schemasystem.inl"
