#include "pch_materialsystem.h"
#include "materialsystem/material.h"
#include "materialsystem/texture.h"
#include "materialsystem/materialvar.h"

/*
==================
CMaterialVar::CMaterialVar
==================
*/
CMaterialVar::CMaterialVar( CMaterial* pMaterial, const char* pName, uint32 id )
	: pName( pName )
	, type( MATERIALVAR_TYPE_UNDEFINED )
	, id( id )
	, pOwningMaterial( pMaterial )
	, onResourceChachedHandle( INVALID_HANDLE )
	, onResourceUncachedHandle( INVALID_HANDLE )
{
}

/*
==================
CMaterialVar::~CMaterialVar
==================
*/
CMaterialVar::~CMaterialVar()
{
	UnsubscribeResourceEvents();
}

/*
==================
CMaterialVar::SetBoolValue
==================
*/
void CMaterialVar::SetBoolValue( bool bValue )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	boolValue				  = bValue;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_BOOL;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetBoolValue
==================
*/
bool CMaterialVar::GetBoolValue() const
{
	switch ( type )
	{
	case MATERIALVAR_TYPE_BOOL: return boolValue;
	case MATERIALVAR_TYPE_INT: return intValue > 0;
	case MATERIALVAR_TYPE_FLOAT: return floatValue > 0.f;
	default: return false;
	}
}

/*
==================
CMaterialVar::SetIntValue
==================
*/
void CMaterialVar::SetIntValue( int32 value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	intValue				  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_INT;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetIntValue
==================
*/
int32 CMaterialVar::GetIntValue() const
{
	switch ( type )
	{
	case MATERIALVAR_TYPE_BOOL: return (int32)boolValue;
	case MATERIALVAR_TYPE_INT: return intValue;
	case MATERIALVAR_TYPE_FLOAT: return (int32)floatValue;
	default: return 0;
	}
}

/*
==================
CMaterialVar::SetFloatValue
==================
*/
void CMaterialVar::SetFloatValue( float value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	floatValue				  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_FLOAT;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetFloatValue
==================
*/
float CMaterialVar::GetFloatValue() const
{
	switch ( type )
	{
	case MATERIALVAR_TYPE_BOOL: return (float)boolValue;
	case MATERIALVAR_TYPE_INT: return (float)intValue;
	case MATERIALVAR_TYPE_FLOAT: return floatValue;
	default: return 0.f;
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const float* pValue, uint32 numComps )
{
	switch ( numComps )
	{
	case 2: SetVecValue( *(vector2_t*)pValue ); break;
	case 3: SetVecValue( *(vector3_t*)pValue ); break;
	case 4: SetVecValue( *(vector4_t*)pValue ); break;
	default:
		AssertMsg( false, "A material variable can take only in range from 2 to 4" );
		break;
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vector2_t& value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	vector2DValue			  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_VECTOR_2D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vector3_t& value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	vector3DValue			  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_VECTOR_3D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vector4_t& value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	vector4DValue			  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_VECTOR_4D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetVecValue
==================
*/
void CMaterialVar::GetVecValue( float* pValue, uint32 numComps ) const
{
	Assert( numComps >= 2 && numComps <= 4 );
	switch ( type )
	{
	case MATERIALVAR_TYPE_VECTOR_2D:
		Assert( numComps >= 2 );
		Mem_Memcpy( pValue, &vector2DValue, numComps * sizeof( float ) );
		break;

	case MATERIALVAR_TYPE_VECTOR_3D:
		Assert( numComps >= 3 );
		Mem_Memcpy( pValue, &vector3DValue, numComps * sizeof( float ) );
		break;

	case MATERIALVAR_TYPE_VECTOR_4D:
		Assert( numComps >= 4 );
		Mem_Memcpy( pValue, &vector4DValue, numComps * sizeof( float ) );
		break;

	default:
		Mem_Memset( pValue, 0, numComps * sizeof( float ) );
		break;
	}
}

/*
==================
CMaterialVar::SetMatrixValue
==================
*/
void CMaterialVar::SetMatrixValue( const matrix4x4_t& value )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	matrixValue				  = value;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_MATRIX;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetMatrixValue
==================
*/
matrix4x4_t CMaterialVar::GetMatrixValue() const
{
	return type == MATERIALVAR_TYPE_MATRIX ? matrixValue : g_matrix44Identity;
}

/*
==================
CMaterialVar::SetStringValue
==================
*/
void CMaterialVar::SetStringValue( const char* pValue )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	stringValue				  = pValue;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_STRING;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::GetStringValue
==================
*/
const char* CMaterialVar::GetStringValue() const
{
	return type == MATERIALVAR_TYPE_STRING ? stringValue.c_str() : "";
}

/*
==================
CMaterialVar::SetTextureValue
==================
*/
void CMaterialVar::SetTextureValue( IResource* pValue )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	pResourceValue			  = pValue;
	type					  = MATERIALVAR_TYPE_TEXTURE;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
	SubscribeResourceEvents();
}

/*
==================
CMaterialVar::GetTextureValue
==================
*/
IResource* CMaterialVar::GetTextureValue() const
{
	return type == MATERIALVAR_TYPE_TEXTURE ? pResourceValue : NULL;
}

/*
==================
CMaterialVar::SetMaterialValue
==================
*/
void CMaterialVar::SetMaterialValue( IResource* pValue )
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	pResourceValue			  = pValue;
	type					  = MATERIALVAR_TYPE_MATERIAL;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
	SubscribeResourceEvents();
}

/*
==================
CMaterialVar::GetMaterialValue
==================
*/
IResource* CMaterialVar::GetMaterialValue() const
{
	return type == MATERIALVAR_TYPE_MATERIAL ? pResourceValue : NULL;
}

/*
==================
CMaterialVar::IsDefined
==================
*/
bool CMaterialVar::IsDefined() const
{
	return type != MATERIALVAR_TYPE_UNDEFINED;
}

/*
==================
CMaterialVar::SetUndefined
==================
*/
void CMaterialVar::SetUndefined()
{
	UnsubscribeResourceEvents();
	materialVarType_t oldType = type;
	pResourceValue			  = NULL;
	type					  = MATERIALVAR_TYPE_UNDEFINED;
	if ( pOwningMaterial )
	{
		pOwningMaterial->ReportVarChanged( this, oldType );
	}
}

/*
==================
CMaterialVar::SubscribeResourceEvents
==================
*/
void CMaterialVar::SubscribeResourceEvents()
{
	// Do nothing if the var type isn't resource or `pResourceValue` isn't valid
	if ( !IsResourceVarType( type ) || !pResourceValue )
	{
		return;
	}

	// Subscribe on resource events `OnCached` and `OnUncached`
	if ( onResourceChachedHandle == INVALID_HANDLE )
	{
		onResourceChachedHandle = pResourceValue->OnCached()->Subscribe( &CMaterialVar::OnResourceCachedUncached, this );
	}
	if ( onResourceUncachedHandle == INVALID_HANDLE )
	{
		onResourceUncachedHandle = pResourceValue->OnUncached()->Subscribe( &CMaterialVar::OnResourceCachedUncached, this );
	}
}

/*
==================
CMaterialVar::UnsubscribeResourceEvents
==================
*/
void CMaterialVar::UnsubscribeResourceEvents()
{
	// Do nothing if the var type isn't resource or `pResourceValue` isn't valid
	if ( !IsResourceVarType( type ) || !pResourceValue )
	{
		return;
	}

	// Unsubscribe from resource events
	pResourceValue->OnCached()->Unsubscribe( onResourceChachedHandle );
	pResourceValue->OnUncached()->Unsubscribe( onResourceUncachedHandle );
	onResourceChachedHandle	 = INVALID_HANDLE;
	onResourceUncachedHandle = INVALID_HANDLE;
}

/*
==================
CMaterialVar::OnResourceCachedUncached
==================
*/
void CMaterialVar::OnResourceCachedUncached( void* pUserData, IResource* pResource )
{
	CMaterialVar* pVar = (CMaterialVar*)pUserData;
	if ( pVar->pOwningMaterial )
	{
		pVar->pOwningMaterial->ReportVarChanged( pVar, pVar->type );
	}
}

/*
==================
CMaterialVar::GetName
==================
*/
const char* CMaterialVar::GetName() const
{
	return pName;
}

/*
==================
CMaterialVar::GetType
==================
*/
materialVarType_t CMaterialVar::GetType() const
{
	return type;
}

/*
==================
CMaterialVar::GetOwningMaterial
==================
*/
IMaterial* CMaterialVar::GetOwningMaterial() const
{
	return pOwningMaterial;
}
