#pragma once

/*
==================
shaderPerDrawVar_t::SetUndefined
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetUndefined()
{
	type		   = SHADER_PARAM_TYPE_UNDEFINED;
	pTextureValue  = NULL;
	pMaterialValue = NULL;
}

/*
==================
shaderPerDrawVar_t::SetBoolValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetBoolValue( bool bValue )
{
	type	  = SHADER_PARAM_TYPE_BOOL;
	boolValue = bValue;
}

/*
==================
shaderPerDrawVar_t::SetIntValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetIntValue( int32 value )
{
	type	 = SHADER_PARAM_TYPE_INT;
	intValue = value;
}

/*
==================
shaderPerDrawVar_t::SetFloatValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetFloatValue( float value )
{
	type	   = SHADER_PARAM_TYPE_FLOAT;
	floatValue = value;
}

/*
==================
shaderPerDrawVar_t::SetVecValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetVecValue( const float* pValue, uint32 numComps )
{
	switch ( numComps )
	{
	case 2: SetVecValue( *(vector2_t*)pValue ); break;
	case 3: SetVecValue( *(vector3_t*)pValue ); break;
	case 4: SetVecValue( *(vector4_t*)pValue ); break;
	default:
		AssertMsg( false, "A shader per-draw variable can take only in range from 2 to 4" );
		break;
	}
}

/*
==================
shaderPerDrawVar_t::SetVecValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetVecValue( const vector2_t& value )
{
	type		  = SHADER_PARAM_TYPE_VECTOR_2D;
	vector2DValue = value;
}

/*
==================
shaderPerDrawVar_t::SetVecValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetVecValue( const vector3_t& value )
{
	type		  = SHADER_PARAM_TYPE_VECTOR_3D;
	vector3DValue = value;
}

/*
==================
shaderPerDrawVar_t::SetVecValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetVecValue( const vector4_t& value )
{
	type		  = SHADER_PARAM_TYPE_VECTOR_4D;
	vector4DValue = value;
}

/*
==================
shaderPerDrawVar_t::SetMatrixValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetMatrixValue( const matrix4x4_t& value )
{
	type		= SHADER_PARAM_TYPE_MATRIX;
	matrixValue = value;
}

/*
==================
shaderPerDrawVar_t::SetStringValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetStringValue( const char* pValue )
{
	type		 = SHADER_PARAM_TYPE_STRING;
	pStringValue = pValue;
}

/*
==================
shaderPerDrawVar_t::SetTextureValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetTextureValue( ITextureResource* pValue )
{
	type		  = SHADER_PARAM_TYPE_TEXTURE;
	pTextureValue = pValue;
}

/*
==================
shaderPerDrawVar_t::SetMaterialValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::SetMaterialValue( IMaterialResource* pValue )
{
	type		   = SHADER_PARAM_TYPE_MATERIAL;
	pMaterialValue = pValue;
}

/*
==================
shaderPerDrawVar_t::GetBoolValue
==================
*/
FORCEINLINE bool shaderPerDrawVar_t::GetBoolValue() const
{
	return boolValue;
}

/*
==================
shaderPerDrawVar_t::GetIntValue
==================
*/
FORCEINLINE int32 shaderPerDrawVar_t::GetIntValue() const
{
	return intValue;
}

/*
==================
shaderPerDrawVar_t::GetFloatValue
==================
*/
FORCEINLINE float shaderPerDrawVar_t::GetFloatValue() const
{
	return floatValue;
}

/*
==================
shaderPerDrawVar_t::GetVecValue
==================
*/
FORCEINLINE void shaderPerDrawVar_t::GetVecValue( float* pValue, uint32 numComps ) const
{
	Assert( numComps >= 2 && numComps <= 4 );
	switch ( type )
	{
	case SHADER_PARAM_TYPE_VECTOR_2D:
		Assert( numComps >= 2 );
		Mem_Memcpy( pValue, &vector2DValue, numComps * sizeof( float ) );
		break;

	case SHADER_PARAM_TYPE_VECTOR_3D:
		Assert( numComps >= 3 );
		Mem_Memcpy( pValue, &vector3DValue, numComps * sizeof( float ) );
		break;

	case SHADER_PARAM_TYPE_VECTOR_4D:
		Assert( numComps >= 4 );
		Mem_Memcpy( pValue, &vector4DValue, numComps * sizeof( float ) );
		break;

	default:
		Mem_Memzero( pValue, numComps );
		break;
	}
}

/*
==================
shaderPerDrawVar_t::GetMatrixValue
==================
*/
FORCEINLINE matrix4x4_t shaderPerDrawVar_t::GetMatrixValue() const
{
	return matrixValue;
}

/*
==================
shaderPerDrawVar_t::GetStringValue
==================
*/
FORCEINLINE const char* shaderPerDrawVar_t::GetStringValue() const
{
	return pStringValue;
}

/*
==================
shaderPerDrawVar_t::GetTextureValue
==================
*/
FORCEINLINE ITextureResource* shaderPerDrawVar_t::GetTextureValue() const
{
	return pTextureValue;
}

/*
==================
shaderPerDrawVar_t::GetMaterialValue
==================
*/
FORCEINLINE IMaterialResource* shaderPerDrawVar_t::GetMaterialValue() const
{
	return pMaterialValue;
}
