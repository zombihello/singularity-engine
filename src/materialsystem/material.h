#pragma once
#include "utils/smatdoc/smat_compiled_doc.h"
#include "materialsystem/ishader.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CMaterialVar;

//-----------------------------------------------------------------------------
// A material resource which is owned by the render thread
//-----------------------------------------------------------------------------
class CMaterialResource : public CStudioRenderResource<CRefCounted<IMaterialResource>>
{
public:
	// IMaterialResource interface
	virtual IShader*			GetShader() const override;
	virtual IShaderContextData* GetContextData() const override;

	CMaterialResource( IShader* pShader, IShaderContextData* pContextData );

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

private:
	// IStudioRenderResource interface
	virtual void ReleaseStudioAPI() override;

	IShader*					pShader;
	CRefPtr<IShaderContextData> pContextData;
};

//-----------------------------------------------------------------------------
// A material
//-----------------------------------------------------------------------------
class CMaterial : public CResourceData<IMaterial>
{
public:
	DECLARE_EVENT( COnStudioResourceChanged, IMaterial* /* pMaterial */ );

	// IResourceData interface
	// Marks all dependent resources as used
	virtual void MarkUsedDependencies() override;

	// Set/clear permanent flag in all dependent resources
	virtual void MakePermanentDependencies() override;
	virtual void ClearPermanentDependencies() override;

	// IMaterial interface
	virtual void		  SetShader( const char* pShaderName ) override;
	virtual IMaterialVar* FindVar( const char* pName ) const override;

	virtual uint32					  GetNumVars() const override;
	virtual IMaterialVar**			  GetVars() const override;
	virtual const char*				  GetShaderName() const override;
	virtual IShader*				  GetShader() const override;
	virtual IMaterialResource*		  GetStudioResource() const override;
	virtual IOnStudioResourceChanged* OnStudioResourceChanged() const override;

	CMaterial( IResource* pResource );
	CMaterial( IResource* pResource, const CSMATCompiledMaterialDoc& smatCompiledDoc );
	~CMaterial();

	void Init( const CSMATCompiledMaterialDoc& smatCompiledDoc );
	void Clear();
	void ReportVarChanged( CMaterialVar* pVar, materialVarType_t oldType );

private:
	typedef eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> materialVarsDict_t;

	void CreateStudioResource();
	void DeleteStudioResource();

	IShader*						 pShader;
	eastl::vector<CMaterialVar*>	 vars;
	eastl::vector<uint32>			 resourceVarIds;
	materialVarsDict_t				 varsDict;
	CRefPtr<CMaterialResource>		 pStudioResource;
	mutable COnStudioResourceChanged onStudioResourceChanged;
};

DECLARE_RESOURCE_TYPE( CMaterial, RESOURCE_TYPE_MATERIAL );