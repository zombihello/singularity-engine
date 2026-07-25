#pragma once
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
	virtual IShader*				 GetShader() const override;
	virtual IPerMaterialContextData* GetPerMaterialContextData() const override;

	CMaterialResource();

	void Update( IShader* pShader, CMaterialVar** pVars );
	void Clear();

	bool				   IsValid() const;
	CStudioRenderCmdFence& GetRenderCmdFence();

private:
	CStudioRenderCmdFence			 renderCmdFence;
	IShader*						 pShader;
	CRefPtr<IPerMaterialContextData> pPerMaterialContextData;
};

//-----------------------------------------------------------------------------
// A material
//-----------------------------------------------------------------------------
class CMaterial : public CResourceData<IMaterial>
{
public:
	// IResourceData interface
	// Collect all resources this data depends on
	virtual void CollectDependencies( IResourceDependencyCollector* pCollector ) const override;

	// IMaterial interface
	virtual void		  Init( const materialInitialData_t& initialData ) override;
	virtual void		  Destroy() override;
	virtual IMaterialVar* FindVar( const char* pName ) const override;

	virtual uint32			   GetNumVars() const override;
	virtual IMaterialVar**	   GetVars() const override;
	virtual const char*		   GetShaderName() const override;
	virtual IShader*		   GetShader() const override;
	virtual IMaterialResource* GetStudioResource() const override;

	CMaterial( IResource* pResource );
	~CMaterial();

	void ReportVarChanged( CMaterialVar* pVar, materialVarType_t oldType );

private:
	typedef eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> materialVarsDict_t;

	void SetShader( const char* pShaderName );
	void UpdateStudioResource();
	void UpdateDependencies();
	void ClearStudioResource();

	bool						 bDirtyStudioResource;
	bool						 bDirtyDependencies;
	bool						 bBatchDependencies;
	IShader*					 pShader;
	eastl::vector<CMaterialVar*> vars;
	eastl::vector<uint32>		 resourceVarIds;
	materialVarsDict_t			 varsDict;
	CRefPtr<CMaterialResource>	 pStudioResource;
};

DECLARE_RESOURCE_TYPE( CMaterial, RESOURCE_TYPE_MATERIAL );
#include "materialsystem/material.inl"
