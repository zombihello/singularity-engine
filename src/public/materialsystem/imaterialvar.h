#pragma once
#include "stdlib/types.h"
#include "stdlib/math/math.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class ITexture;


//-----------------------------------------------------------------------------
// Material variable interface
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


class IMaterialVar
{
public:
	virtual ~IMaterialVar() {}

	virtual void SetUndefined() = 0;
	virtual void SetBoolValue( bool bValue ) = 0;
	virtual void SetIntValue( int32 value ) = 0;
	virtual void SetFloatValue( float value ) = 0;
	virtual void SetVecValue( const float* pValue, uint32 numComps ) = 0;
	virtual void SetVecValue( const vec2_t& value ) = 0;
	virtual void SetVecValue( const vec3_t& value ) = 0;
	virtual void SetVecValue( const vec4_t& value ) = 0;
	virtual void SetMatrixValue( const matrix_t& value ) = 0;
	virtual void SetStringValue( const achar* pValue ) = 0;
	virtual void SetTextureValue( ITexture* pValue ) = 0;
	virtual void SetMaterialValue( IMaterial* pValue ) = 0;
	
	virtual bool IsDefined() const = 0;
	virtual const achar* GetName() const = 0;
	virtual materialVarType_t GetType() const = 0;
	virtual IMaterial* GetOwningMaterial() const = 0;
	virtual bool GetBoolValue() const = 0;
	virtual int32 GetIntValue() const = 0;
	virtual float GetFloatValue() const = 0;
	virtual void GetVecValue( float* pValue, uint32 numComps ) const = 0;
	virtual matrix_t GetMatrixValue() const = 0;
	virtual const achar* GetStringValue() const = 0;
	virtual ITexture* GetTextureValue() const = 0;
	virtual IMaterial* GetMaterialValue() const = 0;
};