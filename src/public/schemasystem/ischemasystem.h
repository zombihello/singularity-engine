#pragma once
#include "appframework/iappsystem.h"
#include "schemasystem/ischematype.h"
#include "schemasystem/schemadesc.h"

//-----------------------------------------------------------------------------
// The schema system interface
//-----------------------------------------------------------------------------
#define SCHEMASYSTEM_INTERFACE_VERSION "SSchemaSystem001"
class ISchemaSystem : public IAppSystem
{
public:
	// Opens the scope a module registers its types into, and closes it again
	// NOTE: A scope has to OUTLIVE every scope that names a type of it.
	//		 Closing one too early will cause in a fatal error
	virtual schemaScopeId_t OpenScope( const char* pName )		  = 0;
	virtual void			CloseScope( schemaScopeId_t scopeId ) = 0;

	// Registering a described type
	virtual void RegisterEnum( const schemaEnumDesc_t& enumDesc, schemaScopeId_t scopeId )				  = 0;
	virtual void RegisterInterface( const schemaInterfaceDesc_t& interfaceDesc, schemaScopeId_t scopeId ) = 0;
	virtual void RegisterStruct( const schemaStructDesc_t& structDesc, schemaScopeId_t scopeId )		  = 0;
	virtual void RegisterClass( const schemaClassDesc_t& classDesc, schemaScopeId_t scopeId )			  = 0;

	// Finds a type by its name or by its alias name
	virtual const ISchemaType* FindType( const char* pName ) const = 0;
	const ISchemaEnum*		   FindEnum( const char* pName ) const;
	const ISchemaStruct*	   FindStruct( const char* pName ) const;
	const ISchemaClass*		   FindClass( const char* pName ) const;
	const ISchemaInterface*	   FindInterface( const char* pName ) const;

	// Iterate over every scope, and over every type registered in one
	virtual uint32			   GetNumScopes() const									  = 0;
	virtual schemaScope_t	   GetScope( uint32 index ) const						  = 0;
	virtual uint32			   GetNumTypes( schemaScopeId_t scopeId ) const			  = 0;
	virtual const ISchemaType* GetType( schemaScopeId_t scopeId, uint32 index ) const = 0;
	virtual uint32			   GetGeneration() const								  = 0;	// Moves on every time a scope closes
};

#include "schemasystem/ischemasystem.inl"
