#pragma once

/*
==================
CSMATMaterialVar::SetName
==================
*/
FORCEINLINE void CSMATMaterialVar::SetName( const achar* pName )
{
	name = pName;
}

/*
==================
CSMATMaterialVar::SetBoolValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetBoolValue( bool bValue )
{
	boolValue = bValue;
	type = SMAT_MATERIAL_VAR_TYPE_BOOL;
}

/*
==================
CSMATMaterialVar::GetBoolValue
==================
*/
FORCEINLINE bool CSMATMaterialVar::GetBoolValue() const
{
	switch ( type )
	{
	case SMAT_MATERIAL_VAR_TYPE_BOOL:	return boolValue;
	case SMAT_MATERIAL_VAR_TYPE_INT:	return intValue > 0;
	case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return floatValue > 0.f;
	default:							return false;
	}
}

/*
==================
CSMATMaterialVar::SetIntValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetIntValue( int32 value )
{
	intValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_INT;
}

/*
==================
CSMATMaterialVar::GetIntValue
==================
*/
FORCEINLINE int32 CSMATMaterialVar::GetIntValue() const
{
	switch ( type )
	{
	case SMAT_MATERIAL_VAR_TYPE_BOOL:	return ( int32 )boolValue;
	case SMAT_MATERIAL_VAR_TYPE_INT:	return intValue;
	case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return ( int32 )floatValue;
	default:							return 0;
	}
}

/*
==================
CSMATMaterialVar::SetFloatValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetFloatValue( float value )
{
	floatValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_FLOAT;
}

/*
==================
CSMATMaterialVar::GetFloatValue
==================
*/
FORCEINLINE float CSMATMaterialVar::GetFloatValue() const
{
	switch ( type )
	{
	case SMAT_MATERIAL_VAR_TYPE_BOOL:	return ( float )boolValue;
	case SMAT_MATERIAL_VAR_TYPE_INT:	return ( float )intValue;
	case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return floatValue;
	default:							return 0.f;
	}
}

/*
==================
CSMATMaterialVar::SetVecValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetVecValue( const float* pValue, uint32 numComps )
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
}

/*
==================
CSMATMaterialVar::SetVecValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetVecValue( const vec2_t& value )
{
	vector2DValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_VECTOR_2D;
}

/*
==================
CSMATMaterialVar::SetVecValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetVecValue( const vec3_t& value )
{
	vector3DValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_VECTOR_3D;
}

/*
==================
CSMATMaterialVar::SetVecValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetVecValue( const vec4_t& value )
{
	vector4DValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_VECTOR_4D;
}

/*
==================
CSMATMaterialVar::GetVecValue
==================
*/
FORCEINLINE void CSMATMaterialVar::GetVecValue( float* pValue, uint32 numComps ) const
{
	Assert( numComps >= 2 && numComps <= 4 );
	switch ( type )
	{
	case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
		Assert( numComps >= 2 );
		Mem_Memcpy( pValue, &vector2DValue, numComps * sizeof( float ) );
		break;

	case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
		Assert( numComps >= 3 );
		Mem_Memcpy( pValue, &vector3DValue, numComps * sizeof( float ) );
		break;

	case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
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
CSMATMaterialVar::SetMatrixValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetMatrixValue( const matrix_t& value )
{
	matrixValue = value;
	type = SMAT_MATERIAL_VAR_TYPE_MATRIX;
}

/*
==================
CSMATMaterialVar::GetMatrixValue
==================
*/
FORCEINLINE matrix_t CSMATMaterialVar::GetMatrixValue() const
{
	return type == SMAT_MATERIAL_VAR_TYPE_MATRIX ? matrixValue : g_matrixIdentity;
}

/*
==================
CSMATMaterialVar::SetStringValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetStringValue( const achar* pValue )
{
	stringValue = pValue;
	pStringValue = stringValue.c_str();
	type = SMAT_MATERIAL_VAR_TYPE_STRING;
}

/*
==================
CSMATMaterialVar::GetStringValue
==================
*/
FORCEINLINE const achar* CSMATMaterialVar::GetStringValue() const
{
	return type == SMAT_MATERIAL_VAR_TYPE_STRING ? pStringValue : "";
}

/*
==================
CSMATMaterialVar::SetTextureValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetTextureValue( const achar* pValue )
{
	stringValue = pValue;
	pTextureValue = stringValue.c_str();
	type = SMAT_MATERIAL_VAR_TYPE_TEXTURE;
}

/*
==================
CSMATMaterialVar::GetTextureValue
==================
*/
FORCEINLINE const achar* CSMATMaterialVar::GetTextureValue() const
{
	return type == SMAT_MATERIAL_VAR_TYPE_TEXTURE ? pTextureValue : "";
}

/*
==================
CSMATMaterialVar::SetMaterialValue
==================
*/
FORCEINLINE void CSMATMaterialVar::SetMaterialValue( const achar* pValue )
{
	stringValue = pValue;
	pMaterialValue = stringValue.c_str();
	type = SMAT_MATERIAL_VAR_TYPE_MATERIAL;
}

/*
==================
CSMATMaterialVar::GetMaterialValue
==================
*/
FORCEINLINE const achar* CSMATMaterialVar::GetMaterialValue() const
{
	return type == SMAT_MATERIAL_VAR_TYPE_MATERIAL ? pMaterialValue : "";
}

/*
==================
CSMATMaterialVar::IsDefined
==================
*/
FORCEINLINE bool CSMATMaterialVar::IsDefined() const
{
	return type != SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
}

/*
==================
CSMATMaterialVar::SetUndefined
==================
*/
FORCEINLINE void CSMATMaterialVar::SetUndefined()
{
	type = SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
}

/*
==================
CSMATMaterialVar::GetName
==================
*/
FORCEINLINE const achar* CSMATMaterialVar::GetName() const
{
	return name.c_str();
}

/*
==================
CSMATMaterialVar::GetType
==================
*/
FORCEINLINE smatMaterialVarType_t CSMATMaterialVar::GetType() const
{
	return type;
}

/*
==================
CSMATMaterialVar::operator=
==================
*/
FORCEINLINE CSMATMaterialVar& CSMATMaterialVar::operator=( const CSMATMaterialVar& right )
{
	Copy( right );
	return *this;
}
