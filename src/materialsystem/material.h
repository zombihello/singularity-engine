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
class CMaterialResource : public CRefCounted<IMaterialResource>
{
public:
	// IMaterialResource interface
	virtual IShader*			GetShader() const override;
	virtual IShaderContextData* GetContextData() const override;

	CMaterialResource();

	void Update( IShader* pShader, CMaterialVar** pVars );
	void Clear();

	bool				   IsValid() const;
	CStudioRenderCmdFence& GetRenderCmdFence();

private:
	CStudioRenderCmdFence		renderCmdFence;
	IShader*					pShader;
	CRefPtr<IShaderContextData> pContextData;
};

//-----------------------------------------------------------------------------
// A material
//-----------------------------------------------------------------------------
class CMaterial : public CResourceData<IMaterial>
{
public:
	// IResourceData interface
	// Marks all dependent resources as used
	virtual void MarkUsedDependencies() override;

	// Set/clear permanent flag in all dependent resources
	virtual void MakePermanentDependencies() override;
	virtual void ClearPermanentDependencies() override;

	// IMaterial interface
	virtual void		  SetShader( const char* pShaderName ) override;
	virtual IMaterialVar* FindVar( const char* pName ) const override;

	virtual uint32			   GetNumVars() const override;
	virtual IMaterialVar**	   GetVars() const override;
	virtual const char*		   GetShaderName() const override;
	virtual IShader*		   GetShader() const override;
	virtual IMaterialResource* GetStudioResource() const override;

	CMaterial( IResource* pResource );
	CMaterial( IResource* pResource, const CSMATCompiledMaterialDoc& smatCompiledDoc );
	~CMaterial();

	void Init( const CSMATCompiledMaterialDoc& smatCompiledDoc );
	void Clear();
	void ReportVarChanged( CMaterialVar* pVar, materialVarType_t oldType );

private:
	typedef eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> materialVarsDict_t;

	void UpdateStudioResource();
	void ClearStudioResource();

	bool						 bDirtyStudioResource;
	IShader*					 pShader;
	eastl::vector<CMaterialVar*> vars;
	eastl::vector<uint32>		 resourceVarIds;
	materialVarsDict_t			 varsDict;
	CRefPtr<CMaterialResource>	 pStudioResource;
};

DECLARE_RESOURCE_TYPE( CMaterial, RESOURCE_TYPE_MATERIAL );
#include "materialsystem/material.inl"