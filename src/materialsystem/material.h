#pragma once
#include "utils/smatdoc/smat_compiled_doc.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdContext;
class CMaterialVar;

//-----------------------------------------------------------------------------
// Material
//-----------------------------------------------------------------------------
class CMaterial : public CResourceData<IMaterial>
{
public:
	// IResourceData interface
	// Marks all dependent resources as used
	virtual void MarkUsedDependencies() override;

	// IMaterial interface
	virtual void		  R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) override;
	virtual void		  R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) override;
	virtual void		  SetShader( const char* pShaderName ) override;
	virtual IMaterialVar* FindVar( const char* pName ) const override;

	virtual uint32				GetNumVars() const override;
	virtual IMaterialVar**		GetVars() const override;
	virtual const char*			GetShaderName() const override;
	virtual IShader*			GetShader() const override;
	virtual IShaderContextData* GetShaderContextData() const override;

	CMaterial( IResource* pResource );
	CMaterial( IResource* pResource, const CSMATCompiledMaterialDoc& smatCompiledDoc );
	~CMaterial();

	void Init( const CSMATCompiledMaterialDoc& smatCompiledDoc );
	void Clear();
	void ReportVarChanged( CMaterialVar* pVar, materialVarType_t oldType );

private:
	typedef eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> materialVarsDict_t;

	void R_UpdateContextData();

	bool						 bDirtyContextData;
	IShader*					 pShader;
	CRefPtr<IShaderContextData>	 pContextData;
	eastl::vector<CMaterialVar*> vars;
	eastl::vector<uint32>		 resourceVarIds;
	materialVarsDict_t			 varsDict;
};

DECLARE_RESOURCE_TYPE( CMaterial, RESOURCE_TYPE_MATERIAL );