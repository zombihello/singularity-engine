#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/tier0.h"
#include "utils/smatdoc/smatvar.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMAT (Singularity Material) files
//-----------------------------------------------------------------------------
class CSMATSourceMaterialDoc
{
public:
	// Save and load a material document
	// NOTE: For use Tier1 must be connected by ConnectTier1 (except LoadFromBuffer)
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void SetShaderName( const char* pShaderName );
	void AddVar( const CSMATMaterialVar& var );
	void RemoveVar( uint32 index );

	uint32								   GetNumVars() const;
	const eastl::vector<CSMATMaterialVar>& GetVars() const;
	const char*							   GetShaderName() const;

	void		SetOutputDir( const char* pOutputDir );
	const char* GetOutputDir() const;

private:
	eastl::string					outputDir;
	eastl::string					shaderName;
	eastl::vector<CSMATMaterialVar> vars;
};

#include "utils/smatdoc/smat_source_doc.inl"
