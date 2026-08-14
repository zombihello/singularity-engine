#pragma once

/*
==================
MaterialVar_MakeBool
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeBool( const char* pName, bool bValue )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_BOOL;
	matVarInfo.boolValue		 = bValue;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeInt
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeInt( const char* pName, int32 value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_INT;
	matVarInfo.intValue			 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeFloat
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeFloat( const char* pName, float value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_FLOAT;
	matVarInfo.floatValue		 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeVec
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeVec( const char* pName, const float* pValue, uint32 numComps )
{
	switch ( numComps )
	{
	case 2: return MaterialVar_MakeVec( pName, *(vector2_t*)pValue );
	case 3: return MaterialVar_MakeVec( pName, *(vector3_t*)pValue );
	case 4: return MaterialVar_MakeVec( pName, *(vector4_t*)pValue );
	default:
		AssertMsg( false, "A material variable info can take only in range from 2 to 4" );
		return materialVarInfo_t{};
	}
}

/*
==================
MaterialVar_MakeVec
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector2_t& value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_VECTOR_2D;
	matVarInfo.vector2DValue	 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeVec
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector3_t& value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_VECTOR_3D;
	matVarInfo.vector3DValue	 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeVec
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector4_t& value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_VECTOR_4D;
	matVarInfo.vector4DValue	 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeMatrix
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeMatrix( const char* pName, const matrix4x4_t& value )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_MATRIX;
	matVarInfo.matrixValue		 = value;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeString
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeString( const char* pName, const char* pValue )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_STRING;
	matVarInfo.pStringValue		 = pValue;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeTexture
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeTexture( const char* pName, IResource* pValue )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_TEXTURE;
	matVarInfo.pResourceValue	 = pValue;
	return matVarInfo;
}

/*
==================
MaterialVar_MakeMaterial
==================
*/
FORCEINLINE materialVarInfo_t MaterialVar_MakeMaterial( const char* pName, IResource* pValue )
{
	materialVarInfo_t matVarInfo = {};
	matVarInfo.pName			 = pName;
	matVarInfo.type				 = MATERIALVAR_TYPE_MATERIAL;
	matVarInfo.pResourceValue	 = pValue;
	return matVarInfo;
}
