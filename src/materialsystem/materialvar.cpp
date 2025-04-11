#include "pch_materialsystem.h"
#include "materialsystem/material.h"
#include "materialsystem/materialvar.h"

/*
==================
CMaterialVar::CMaterialVar
==================
*/
CMaterialVar::CMaterialVar( IMaterial* pMaterial, const achar* pName )
	: pName( pName )
	, type( MATERIALVAR_TYPE_UNDEFINED )
	, pOwningMaterial( ( CMaterial* )pMaterial )
{}

/*
==================
CMaterialVar::~CMaterialVar
==================
*/
CMaterialVar::~CMaterialVar()
{}

/*
==================
CMaterialVar::SetBoolValue
==================
*/
void CMaterialVar::SetBoolValue( bool bValue )
{
	boolValue	= bValue;
	type		= MATERIALVAR_TYPE_BOOL;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
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
	case MATERIALVAR_TYPE_BOOL:		return boolValue;
	case MATERIALVAR_TYPE_INT:		return intValue > 0;
	case MATERIALVAR_TYPE_FLOAT:	return floatValue > 0.f;
	default:						return false;
	}
}

/*
==================
CMaterialVar::SetIntValue
==================
*/
void CMaterialVar::SetIntValue( int32 value )
{
	intValue	= value;
	type		= MATERIALVAR_TYPE_INT;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
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
	case MATERIALVAR_TYPE_BOOL:		return ( int32 )boolValue;
	case MATERIALVAR_TYPE_INT:		return intValue;
	case MATERIALVAR_TYPE_FLOAT:	return ( int32 )floatValue;
	default:						return 0;
	}
}

/*
==================
CMaterialVar::SetFloatValue
==================
*/
void CMaterialVar::SetFloatValue( float value )
{
	floatValue	= value;
	type		= MATERIALVAR_TYPE_FLOAT;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
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
	case MATERIALVAR_TYPE_BOOL:		return ( float )boolValue;
	case MATERIALVAR_TYPE_INT:		return ( float )intValue;
	case MATERIALVAR_TYPE_FLOAT:	return floatValue;
	default:						return 0.f;
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
	case 2: SetVecValue( *( vec2_t* )pValue ); break;
	case 3: SetVecValue( *( vec3_t* )pValue ); break;
	case 4: SetVecValue( *( vec4_t* )pValue ); break;
	default:
		AssertMsg( false, "A material variable can take only in range from 2 to 4" );
		break;
	}

	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vec2_t& value )
{
	vector2DValue	= value;
	type			= MATERIALVAR_TYPE_VECTOR_2D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vec3_t& value )
{
	vector3DValue	= value;
	type			= MATERIALVAR_TYPE_VECTOR_3D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::SetVecValue
==================
*/
void CMaterialVar::SetVecValue( const vec4_t& value )
{
	vector4DValue	= value;
	type			= MATERIALVAR_TYPE_VECTOR_4D;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
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
void CMaterialVar::SetMatrixValue( const matrix_t& value )
{
	matrixValue = value;
	type		= MATERIALVAR_TYPE_MATRIX;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::GetMatrixValue
==================
*/
matrix_t CMaterialVar::GetMatrixValue() const
{
	return type == MATERIALVAR_TYPE_MATRIX ? matrixValue : math_t::matrixIdentity;
}

/*
==================
CMaterialVar::SetStringValue
==================
*/
void CMaterialVar::SetStringValue( const achar* pValue )
{
	stringValue = pValue;
	type		= MATERIALVAR_TYPE_STRING;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::GetStringValue
==================
*/
const achar* CMaterialVar::GetStringValue() const
{
	return type == MATERIALVAR_TYPE_STRING ? stringValue.c_str() : "";
}

/*
==================
CMaterialVar::SetTextureValue
==================
*/
void CMaterialVar::SetTextureValue( ITexture* pValue )
{
	pTextureValue	= pValue;
	type			= MATERIALVAR_TYPE_TEXTURE;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::GetTextureValue
==================
*/
ITexture* CMaterialVar::GetTextureValue() const
{
	return type == MATERIALVAR_TYPE_TEXTURE ? pTextureValue : NULL;
}

/*
==================
CMaterialVar::SetMaterialValue
==================
*/
void CMaterialVar::SetMaterialValue( IMaterial* pValue )
{
	pMaterialValue	= ( CMaterial* )pValue;
	type			= MATERIALVAR_TYPE_MATERIAL;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::GetMaterialValue
==================
*/
IMaterial* CMaterialVar::GetMaterialValue() const
{
	return type == MATERIALVAR_TYPE_MATERIAL ? pMaterialValue : NULL;
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
	type = MATERIALVAR_TYPE_UNDEFINED;
	if ( pOwningMaterial )
	{
		pOwningMaterial->MarkDirtyBuffers();
	}
}

/*
==================
CMaterialVar::GetName
==================
*/
const achar* CMaterialVar::GetName() const
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