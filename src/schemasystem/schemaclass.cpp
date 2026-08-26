#include "pch_schemasystem.h"
#include "schemasystem/ischemaobject.h"
#include "schemasystem/ischemainterface.h"
#include "schemasystem/schemaclass.h"

/*
==================
CSchemaClass::CSchemaClass
==================
*/
CSchemaClass::CSchemaClass( const schemaClassDesc_t& classDesc )
	: CSchemaCompoundTypeBase<ISchemaClass>( SCHEMA_TYPE_KIND_CLASS, classDesc )
	, objectOffset( classDesc.objectOffset )
	, numBaseInterfaces( 0 )
{
	interfaces.reserve( classDesc.numInterfaces );
	for ( uint32 index = 0; index < classDesc.numInterfaces; ++index )
	{
		implementedInterface_t&					interface	  = interfaces.emplace_back();
		const schemaImplementedInterfaceDesc_t& interfaceDesc = classDesc.pInterfaces[index];
		interface.pTypeName									  = interfaceDesc.pTypeName;
		interface.offset									  = interfaceDesc.offset;
		interface.pInterface								  = NULL;
	}
}

/*
==================
CSchemaClass::OnResolve
==================
*/
void CSchemaClass::OnResolve()
{
	// Resolve the parent class first
	PROFILER_SCOPE_FUNC();
	CSchemaCompoundTypeBase<ISchemaClass>::OnResolve();

	// Resolve interfaces
	for ( uint32 index = 0, count = (uint32)interfaces.size(); index < count; ++index )
	{
		// Try to resolve the interface
		implementedInterface_t& interface	  = interfaces[index];
		ISchemaTypeInternal*	pTypeInternal = ResolveDependency( interface.pTypeName );
		if ( !pTypeInternal )
		{
			Sys_Error( "'%s' implements '%s' but that type has no schema description", GetName(), interface.pTypeName );
			return;
		}

		// Make sure that the type is an interface
		ISchemaType* pSchemaType = pTypeInternal->AsISchemaType();
		interface.pInterface	 = pSchemaType->As<ISchemaInterface>();
		if ( !interface.pInterface )
		{
			Sys_Error( "'%s' implements '%s', which is a %s and not an interface", GetName(), interface.pTypeName, Schema_ConvTypeKindToString( pSchemaType->GetKind() ) );
			return;
		}
	}

	// Inherit number of interfaces from the base class
	const ISchemaClass* pBaseClass = GetBaseClass();
	if ( pBaseClass )
	{
		numBaseInterfaces = pBaseClass->GetNumInterfaces( true );
	}
}

/*
==================
CSchemaClass::OnUnresolve
==================
*/
void CSchemaClass::OnUnresolve()
{
	// Reset all resolved fields
	PROFILER_SCOPE_FUNC();
	CSchemaCompoundTypeBase<ISchemaClass>::OnUnresolve();
	for ( uint32 index = 0, count = (uint32)interfaces.size(); index < count; ++index )
	{
		interfaces[index].pInterface = NULL;
	}
	numBaseInterfaces = 0;
}

/*
==================
CSchemaClass::Construct
==================
*/
ISchemaObject* CSchemaClass::Construct( void* pMemory ) const
{
	PROFILER_SCOPE_FUNC();
	CheckCanConstruct( false );
	GetObjectOps()->pConstructFn( pMemory );
	return (ISchemaObject*)( (byte*)pMemory + objectOffset );
}

/*
==================
CSchemaClass::Destruct
==================
*/
void CSchemaClass::Destruct( void* pObject ) const
{
	PROFILER_SCOPE_FUNC();
	if ( pObject )
	{
		GetObjectOps()->pDestructFn( pObject );
	}
}

/*
==================
CSchemaClass::New
==================
*/
ISchemaObject* CSchemaClass::New() const
{
	PROFILER_SCOPE_FUNC();
	CheckCanConstruct();
	void* pRawObject = GetObjectOps()->pNewFn();
	return (ISchemaObject*)( (byte*)pRawObject + objectOffset );
}

/*
==================
CSchemaClass::Delete
==================
*/
void CSchemaClass::Delete( ISchemaObject* pObject ) const
{
	PROFILER_SCOPE_FUNC();
	if ( pObject && !HasAnyFlags( SCHEMA_TYPE_FLAG_REFCOUNTED ) )
	{
		GetObjectOps()->pDeleteFn( pObject->GetSchemaRawPtr() );
	}
}

/*
==================
CSchemaClass::IsA
==================
*/
bool CSchemaClass::IsA( const ISchemaType* pType ) const
{
	// Do nothing if the target type isn't valid or not a class/interface
	PROFILER_SCOPE_FUNC();
	if ( !pType )
	{
		return false;
	}

	schemaTypeKind_t targetKind = pType->GetKind();
	if ( targetKind != SCHEMA_TYPE_KIND_CLASS && targetKind != SCHEMA_TYPE_KIND_INTERFACE )
	{
		return false;
	}
	bool bIsInterface = targetKind == SCHEMA_TYPE_KIND_INTERFACE;

	// Check all our base classes (and ourselves)
	for ( const ISchemaClass* pCurType = this; pCurType; pCurType = pCurType->GetBaseClass() )
	{
		if ( pCurType == pType )
		{
			return true;
		}
		if ( !bIsInterface )
		{
			continue;
		}

		// Check the interfaces that step implements
		for ( uint32 index = 0, count = pCurType->GetNumInterfaces(); index < count; ++index )
		{
			const ISchemaInterface* pInterface = pCurType->GetInterface( index );
			if ( pInterface && pInterface->IsA( pType ) )
			{
				return true;
			}
		}
	}

	// The class isn't the target type
	return false;
}

/*
==================
CSchemaClass::Cast
==================
*/
void* CSchemaClass::Cast( void* pObject, const ISchemaType* pType ) const
{
	// Do nothing if the target type/the object isn't valid or not a class/interface
	PROFILER_SCOPE_FUNC();
	if ( !pObject || !pType )
	{
		return NULL;
	}
	if ( pType == this )
	{
		return pObject;
	}

	schemaTypeKind_t targetKind = pType->GetKind();
	if ( targetKind != SCHEMA_TYPE_KIND_CLASS && targetKind != SCHEMA_TYPE_KIND_INTERFACE )
	{
		return NULL;
	}

	// Try cast to an interface
	if ( targetKind == SCHEMA_TYPE_KIND_INTERFACE )
	{
		for ( uint32 index = 0, count = (uint32)interfaces.size(); index < count; ++index )
		{
			const implementedInterface_t& interface = interfaces[index];
			if ( interface.pInterface && interface.pInterface->IsA( pType ) )
			{
				return (byte*)pObject + interface.offset;
			}
		}
	}

	// The adjustment recorded for every step is what makes this correct under
	// multiple inheritance
	const ISchemaClass* pBaseType = GetBaseClass();
	return pBaseType ? pBaseType->Cast( (byte*)pObject + GetBaseOffset(), pType ) : NULL;
}

/*
==================
CSchemaClass::Cast
==================
*/
void* CSchemaClass::Cast( ISchemaObject* pObject, const ISchemaType* pType ) const
{
	// Do nothing if the target type/the object isn't valid
	PROFILER_SCOPE_FUNC();
	if ( !pObject || !pType )
	{
		return NULL;
	}

	// Delegate the cast to the object's actual class if it differs from this class
	const ISchemaClass* pObjectClass = pObject->GetSchemaClass();
	Assert( pObjectClass );
	if ( pObjectClass != this )
	{
		return pObjectClass->Cast( pObject, pType );
	}

	// Otherwise cast the object by the class
	return Cast( pObject->GetSchemaRawPtr(), pType );
}

/*
==================
CSchemaClass::GetNumInterfaces
==================
*/
uint32 CSchemaClass::GetNumInterfaces( bool bWithParents /* = false */ ) const
{
	return bWithParents ? (uint32)interfaces.size() + numBaseInterfaces : (uint32)interfaces.size();
}

/*
==================
CSchemaClass::GetInterface
==================
*/
const ISchemaInterface* CSchemaClass::GetInterface( uint32 index, bool bWithParents /* = false */ ) const
{
	PROFILER_SCOPE_FUNC();
	if ( bWithParents )
	{
		if ( index < numBaseInterfaces )
		{
			const ISchemaClass* pBaseType = GetBaseClass();
			Assert( pBaseType );
			return pBaseType->GetInterface( index, true );
		}
		index -= numBaseInterfaces;
	}

	Assert( index < (uint32)interfaces.size() );
	return interfaces[index].pInterface;
}
