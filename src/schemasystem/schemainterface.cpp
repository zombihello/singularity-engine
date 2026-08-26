#include "pch_schemasystem.h"
#include "schemasystem/schemainterface.h"

/*
==================
CSchemaInterface::CSchemaInterface
==================
*/
CSchemaInterface::CSchemaInterface( const schemaInterfaceDesc_t& interfaceDesc )
	: CSchemaTypeBase<ISchemaInterface>( SCHEMA_TYPE_KIND_INTERFACE, interfaceDesc.pName, interfaceDesc.pAliasName, interfaceDesc.flags, 0, 0, interfaceDesc.pMetadata, interfaceDesc.numMetadata )
	, baseFlags( SCHEMA_TYPE_FLAG_NONE )
	, pBaseTypeName( interfaceDesc.pBaseTypeName )
	, pBaseInterface( NULL )
{
}

/*
==================
CSchemaInterface::OnResolve
==================
*/
void CSchemaInterface::OnResolve()
{
	// Nothing to settle when the interface has no base
	PROFILER_SCOPE_FUNC();
	if ( !pBaseTypeName || !pBaseTypeName[0] )
	{
		return;
	}

	// Try to resolve the base type
	ISchemaTypeInternal* pBaseTypeInternal = ResolveDependency( pBaseTypeName );
	if ( !pBaseTypeInternal )
	{
		Sys_Error( "The base of '%s' is declared as '%s' but that type has no schema description", GetName(), pBaseTypeName );
		return;
	}

	// Make sure that the base type is interface, because an interface may only derive from an interface
	ISchemaType* pBaseSchemaType = pBaseTypeInternal->AsISchemaType();
	pBaseInterface				 = pBaseSchemaType->As<ISchemaInterface>();
	if ( !pBaseInterface )
	{
		Sys_Error( "The base of '%s' is declared as '%s', which is a %s and not an interface. A schema type may only derive from its own kind", GetName(), pBaseTypeName, Schema_ConvTypeKindToString( pBaseSchemaType->GetKind() ) );
		return;
	}

	// Inherit flags from the base interface
	baseFlags = pBaseInterface->GetFlags() & SCHEMA_TYPE_MASK_FLAGS_INHERITED;
}

/*
==================
CSchemaInterface::OnUnresolve
==================
*/
void CSchemaInterface::OnUnresolve()
{
	// Reset all resolved fields
	PROFILER_SCOPE_FUNC();
	baseFlags	   = SCHEMA_TYPE_FLAG_NONE;
	pBaseInterface = NULL;
}

/*
==================
CSchemaInterface::IsA
==================
*/
bool CSchemaInterface::IsA( const ISchemaType* pType ) const
{
	// Do nothing if the target type isn't valid or not an interface
	PROFILER_SCOPE_FUNC();
	if ( !pType || pType->GetKind() != SCHEMA_TYPE_KIND_INTERFACE )
	{
		return false;
	}

	// Check all our base interfaces (and ourselves)
	for ( const ISchemaInterface* pCurType = this; pCurType; pCurType = pCurType->GetBaseInterface() )
	{
		if ( pCurType == pType )
		{
			return true;
		}
	}
	return false;
}

/*
==================
CSchemaInterface::GetFlags
==================
*/
uint32 CSchemaInterface::GetFlags() const
{
	return CSchemaTypeBase<ISchemaInterface>::GetFlags() | baseFlags;
}

/*
==================
CSchemaInterface::GetBaseInterface
==================
*/
const ISchemaInterface* CSchemaInterface::GetBaseInterface() const
{
	return pBaseInterface;
}
