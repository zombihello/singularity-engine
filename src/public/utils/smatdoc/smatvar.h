#pragma once
#include <EASTL/string.h>

#include "tier0/tier0.h"
#include "tier0/debug.h"
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// SMAT material variable
//-----------------------------------------------------------------------------
enum smatMaterialVarType_t
{
	SMAT_MATERIAL_VAR_TYPE_UNDEFINED,
	SMAT_MATERIAL_VAR_TYPE_BOOL,
	SMAT_MATERIAL_VAR_TYPE_INT,
	SMAT_MATERIAL_VAR_TYPE_FLOAT,
	SMAT_MATERIAL_VAR_TYPE_VECTOR_2D,
	SMAT_MATERIAL_VAR_TYPE_VECTOR_3D,
	SMAT_MATERIAL_VAR_TYPE_VECTOR_4D,
	SMAT_MATERIAL_VAR_TYPE_MATRIX,
	SMAT_MATERIAL_VAR_TYPE_STRING,
	SMAT_MATERIAL_VAR_TYPE_TEXTURE,
	SMAT_MATERIAL_VAR_TYPE_MATERIAL,
	SMAT_MATERIAL_VAR_NUM_TYPES
};

class CSMATMaterialVar
{
public:
	CSMATMaterialVar();
	CSMATMaterialVar( const CSMATMaterialVar& other );
	~CSMATMaterialVar();

	void Copy( const CSMATMaterialVar& other );

	void SetName( const char* pName );
	void SetUndefined();
	void SetBoolValue( bool bValue );
	void SetIntValue( int32 value );
	void SetFloatValue( float value );
	void SetVecValue( const float* pValue, uint32 numComps );
	void SetVecValue( const vec2_t& value );
	void SetVecValue( const vec3_t& value );
	void SetVecValue( const vec4_t& value );
	void SetMatrixValue( const mat4_t& value );
	void SetStringValue( const char* pValue );
	void SetTextureValue( const char* pValue );
	void SetMaterialValue( const char* pValue );

	bool				  IsDefined() const;
	const char*			  GetName() const;
	smatMaterialVarType_t GetType() const;
	bool				  GetBoolValue() const;
	int32				  GetIntValue() const;
	float				  GetFloatValue() const;
	void				  GetVecValue( float* pValue, uint32 numComps ) const;
	mat4_t				  GetMatrixValue() const;
	const char*			  GetStringValue() const;
	const char*			  GetTextureValue() const;
	const char*			  GetMaterialValue() const;

	CSMATMaterialVar& operator=( const CSMATMaterialVar& right );

private:
	eastl::string		  name;
	smatMaterialVarType_t type;
	eastl::string		  stringValue;
	union
	{
		bool		boolValue;
		int32		intValue;
		float		floatValue;
		vec2_t		vector2DValue;
		vec3_t		vector3DValue;
		vec4_t		vector4DValue;
		mat4_t		matrixValue;
		const char* pStringValue;
		const char* pTextureValue;
		const char* pMaterialValue;
	};
};

#include "utils/smatdoc/smatvar.inl"
