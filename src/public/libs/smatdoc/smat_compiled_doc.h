#pragma once
#include <string>
#include <vector>

#include "core/core.h"
#include "smatdoc/smatvar.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMAT (Singularity Material) files
//-----------------------------------------------------------------------------
class CSMATCompiledMaterialDoc
{
public:
	// Save and load a material document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool SaveFile( const achar* pPath );
	bool LoadFromFile( const achar* pPath );

	void SetShaderName( const achar* pShaderName );
	void AddVar( const CSMATMaterialVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	uint32 GetNumVars() const;
	const std::vector<CSMATMaterialVar>& GetVars() const;
	const achar* GetShaderName() const;

private:
	std::string						shaderName;
	std::vector<CSMATMaterialVar>	vars;
};

#include "smatdoc/smat_compiled_doc.inl"