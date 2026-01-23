#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "core/core.h"
#include "utils/smatdoc/smatvar.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMAT (Singularity Material) files
//-----------------------------------------------------------------------------
class CSMATCompiledMaterialDoc
{
public:
	// Save and load a material document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool SaveFile( const char* pPath );
	bool LoadFromFile( const char* pPath );

	void SetShaderName( const char* pShaderName );
	void AddVar( const CSMATMaterialVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	uint32								   GetNumVars() const;
	const eastl::vector<CSMATMaterialVar>& GetVars() const;
	const char*							   GetShaderName() const;

private:
	eastl::string					shaderName;
	eastl::vector<CSMATMaterialVar> vars;
};

#include "utils/smatdoc/smat_compiled_doc.inl"
