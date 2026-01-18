#pragma once
#include "materialsystem/imaterialvar.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CMaterial;

//-----------------------------------------------------------------------------
// Material variable
//-----------------------------------------------------------------------------
class CMaterialVar : public IMaterialVar
{
public:
	CMaterialVar( IMaterial* pMaterial, const char* pName );
	~CMaterialVar();

	// IMaterialVar interface
	virtual void SetUndefined() override;
	virtual void SetBoolValue( bool bValue ) override;
	virtual void SetIntValue( int32 value ) override;
	virtual void SetFloatValue( float value ) override;
	virtual void SetVecValue( const float* pValue, uint32 numComps ) override;
	virtual void SetVecValue( const vec2_t& value ) override;
	virtual void SetVecValue( const vec3_t& value ) override;
	virtual void SetVecValue( const vec4_t& value ) override;
	virtual void SetMatrixValue( const matrix_t& value ) override;
	virtual void SetStringValue( const char* pValue ) override;
	virtual void SetTextureValue( ITexture* pValue ) override;
	virtual void SetMaterialValue( IMaterial* pValue ) override;

	virtual bool			  IsDefined() const override;
	virtual const char*	  GetName() const override;
	virtual materialVarType_t GetType() const override;
	virtual IMaterial*		  GetOwningMaterial() const override;
	virtual bool			  GetBoolValue() const override;
	virtual int32			  GetIntValue() const override;
	virtual float			  GetFloatValue() const override;
	virtual void			  GetVecValue( float* pValue, uint32 numComps ) const override;
	virtual matrix_t		  GetMatrixValue() const override;
	virtual const char*	  GetStringValue() const override;
	virtual ITexture*		  GetTextureValue() const override;
	virtual IMaterial*		  GetMaterialValue() const override;

private:
	const char*	  pName;
	materialVarType_t type;
	CMaterial*		  pOwningMaterial;
	union
	{
		bool			   boolValue;
		int32			   intValue;
		float			   floatValue;
		vec2_t			   vector2DValue;
		vec3_t			   vector3DValue;
		vec4_t			   vector4DValue;
		matrix_t		   matrixValue;
		std::string		   stringValue;
		ITexture*		   pTextureValue;
		TRefPtr<CMaterial> pMaterialValue;
	};
};