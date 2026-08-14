#pragma once
#include "tier0/types.h"
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IResource;
class IMaterial;

//-----------------------------------------------------------------------------
// Material variable types
//-----------------------------------------------------------------------------
enum materialVarType_t
{
	MATERIALVAR_TYPE_UNDEFINED,
	MATERIALVAR_TYPE_BOOL,
	MATERIALVAR_TYPE_INT,
	MATERIALVAR_TYPE_FLOAT,
	MATERIALVAR_TYPE_VECTOR_2D,
	MATERIALVAR_TYPE_VECTOR_3D,
	MATERIALVAR_TYPE_VECTOR_4D,
	MATERIALVAR_TYPE_MATRIX,
	MATERIALVAR_TYPE_STRING,
	MATERIALVAR_TYPE_TEXTURE,
	MATERIALVAR_TYPE_MATERIAL
};

//-----------------------------------------------------------------------------
// A material variable info which is used to initialize a material
//-----------------------------------------------------------------------------
struct materialVarInfo_t
{
	const char*		  pName;
	materialVarType_t type;
	union
	{
		bool		boolValue;
		int32		intValue;
		float		floatValue;
		vector2_t	vector2DValue;
		vector3_t	vector3DValue;
		vector4_t	vector4DValue;
		matrix4x4_t matrixValue;
		const char* pStringValue;
		IResource*	pResourceValue;
	};
};

//-----------------------------------------------------------------------------
// Material variable interface
//-----------------------------------------------------------------------------
class IMaterialVar
{
public:
	virtual ~IMaterialVar() {}

	virtual void SetUndefined()										 = 0;
	virtual void SetBoolValue( bool bValue )						 = 0;
	virtual void SetIntValue( int32 value )							 = 0;
	virtual void SetFloatValue( float value )						 = 0;
	virtual void SetVecValue( const float* pValue, uint32 numComps ) = 0;
	virtual void SetVecValue( const vector2_t& value )				 = 0;
	virtual void SetVecValue( const vector3_t& value )				 = 0;
	virtual void SetVecValue( const vector4_t& value )				 = 0;
	virtual void SetMatrixValue( const matrix4x4_t& value )			 = 0;
	virtual void SetStringValue( const char* pValue )				 = 0;
	virtual void SetTextureValue( IResource* pValue )				 = 0;
	virtual void SetMaterialValue( IResource* pValue )				 = 0;

	virtual bool			  IsDefined() const									  = 0;
	virtual const char*		  GetName() const									  = 0;
	virtual materialVarType_t GetType() const									  = 0;
	virtual IMaterial*		  GetOwningMaterial() const							  = 0;
	virtual bool			  GetBoolValue() const								  = 0;
	virtual int32			  GetIntValue() const								  = 0;
	virtual float			  GetFloatValue() const								  = 0;
	virtual void			  GetVecValue( float* pValue, uint32 numComps ) const = 0;
	virtual matrix4x4_t		  GetMatrixValue() const							  = 0;
	virtual const char*		  GetStringValue() const							  = 0;
	virtual IResource*		  GetTextureValue() const							  = 0;
	virtual IResource*		  GetMaterialValue() const							  = 0;
};

//-----------------------------------------------------------------------------
// Functions to make a material variable info
//-----------------------------------------------------------------------------
materialVarInfo_t MaterialVar_MakeBool( const char* pName, bool bValue );
materialVarInfo_t MaterialVar_MakeInt( const char* pName, int32 value );
materialVarInfo_t MaterialVar_MakeFloat( const char* pName, float value );
materialVarInfo_t MaterialVar_MakeVec( const char* pName, const float* pValue, uint32 numComps );
materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector2_t& value );
materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector3_t& value );
materialVarInfo_t MaterialVar_MakeVec( const char* pName, const vector4_t& value );
materialVarInfo_t MaterialVar_MakeMatrix( const char* pName, const matrix4x4_t& value );
materialVarInfo_t MaterialVar_MakeString( const char* pName, const char* pValue );
materialVarInfo_t MaterialVar_MakeTexture( const char* pName, IResource* pValue );
materialVarInfo_t MaterialVar_MakeMaterial( const char* pName, IResource* pValue );

#include "materialsystem/imaterialvar.inl"
