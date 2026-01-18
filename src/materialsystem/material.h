#pragma once
#include "smatdoc/smat_compiled_doc.h"
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/istudio_rendercmd.h"
#include "materialsystem/imaterial.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPICmdContext;
class CMaterialVar;

//-----------------------------------------------------------------------------
// Material
//-----------------------------------------------------------------------------
class CMaterial : public TRefCounted<IMaterial>
{
public:
	// IMaterial interface
	virtual void		  R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) override;
	virtual void		  R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) override;
	virtual void		  SetShader( const char* pShaderName ) override;
	virtual IMaterialVar* FindVar( const char* pName ) const override;

	virtual uint32		   GetNumVars() const override;
	virtual IMaterialVar** GetVars() const override;
	virtual const char*   GetShaderName() const override;
	virtual IShader*	   GetShader() const override;

	CMaterial();
	CMaterial( const CSMATCompiledMaterialDoc& smatCompiledDoc );
	~CMaterial();

	void			 Clear();
	FORCEINLINE void MarkDirtyBuffers()
	{
		bDirtyBuffers = true;
	}

private:
	// Calculate a hash for a string to use it in std::unordered_map
	struct insensitiveStringHash_t
	{
		std::size_t operator()( const char* pString ) const;
	};

	// Comparator for std::unordered_map to insensitive compre strings
	struct insensitiveCompareString_t
	{
		bool operator()( const char* pLeft, const char* pRight ) const;
	};

	typedef std::unordered_map<const char*, uint32, insensitiveStringHash_t, insensitiveCompareString_t> materialVarsDict_t;

	void Init( const CSMATCompiledMaterialDoc& smatCompiledDoc );
	void R_UpdateBuffers( IStudioAPICmdContext* pCmdContext );
	void DestroyBuffers();

	bool								   bDirtyBuffers;
	IShader*							   pShader;
	std::vector<CMaterialVar*>			   vars;
	materialVarsDict_t					   varsDict;
	std::vector<TRefPtr<IStudioAPIBuffer>> studioAPIBuffers;
};

#include "materialsystem/material.inl"