#pragma once
#include "materialsystem/texture.h"
#include "materialsystem/material.h"
#include "resourcesystem/resourceptr.h"

//-----------------------------------------------------------------------------
// Material variable
//-----------------------------------------------------------------------------
class CMaterialVar : public IMaterialVar
{
public:
	// IMaterialVar interface
	virtual void SetUndefined() override;
	virtual void SetBoolValue( bool bValue ) override;
	virtual void SetIntValue( int32 value ) override;
	virtual void SetFloatValue( float value ) override;
	virtual void SetVecValue( const float* pValue, uint32 numComps ) override;
	virtual void SetVecValue( const vector2_t& value ) override;
	virtual void SetVecValue( const vector3_t& value ) override;
	virtual void SetVecValue( const vector4_t& value ) override;
	virtual void SetMatrixValue( const matrix4x4_t& value ) override;
	virtual void SetStringValue( const char* pValue ) override;
	virtual void SetTextureValue( IResource* pValue ) override;
	virtual void SetMaterialValue( IResource* pValue ) override;

	virtual bool			  IsDefined() const override;
	virtual const char*		  GetName() const override;
	virtual materialVarType_t GetType() const override;
	virtual IMaterial*		  GetOwningMaterial() const override;
	virtual bool			  GetBoolValue() const override;
	virtual int32			  GetIntValue() const override;
	virtual float			  GetFloatValue() const override;
	virtual void			  GetVecValue( float* pValue, uint32 numComps ) const override;
	virtual matrix4x4_t		  GetMatrixValue() const override;
	virtual const char*		  GetStringValue() const override;
	virtual IResource*		  GetTextureValue() const override;
	virtual IResource*		  GetMaterialValue() const override;

	CMaterialVar( CMaterial* pMaterial, const char* pName, uint32 id );
	~CMaterialVar();

	static bool IsResourceVarType( materialVarType_t varType );
	uint32		GetId() const;

private:
	static void OnResourceCachedUncached( void* pUserData, IResource* pResource );
	static void OnTextureResourceChanged( void* pUserData, ITexture* pTexture );
	void		SubscribeResourceEvents();
	void		UnsubscribeResourceEvents( bool bOnlyResourceDataEvents = false );

	const char*		  pName;
	materialVarType_t type;
	uint32			  id;
	CMaterial*		  pOwningMaterial;
	union
	{
		bool		boolValue;
		int32		intValue;
		float		floatValue;
		vector2_t	vector2DValue;
		vector3_t	vector3DValue;
		vector4_t	vector4DValue;
		matrix4x4_t matrixValue;
	};
	eastl::string					 stringValue;
	CRefPtr<IResource>				 pResourceValue;
	IResource::IOnCached::handle_t	 onResourceChachedHandle;
	IResource::IOnUncached::handle_t onResourceUncachedHandle;
	union
	{
		CTexture::COnStudioResourceChanged::handle_t onTextureResourceChangedHandle;
	};
};

#include "materialsystem/materialvar.inl"
