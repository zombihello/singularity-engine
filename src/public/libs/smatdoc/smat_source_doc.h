#pragma once
#include <string>
#include <vector>

#include "core/core.h"
#include "stdlib/jsondoc.h"
#include "smatdoc/smatvar.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMAT (Singularity Material) files
//-----------------------------------------------------------------------------
class CSMATSourceMaterialDoc
{
public:
	// Save and load a material document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer)
	bool SaveFile( const achar* pPath );
	bool LoadFromFile( const achar* pPath );
	bool LoadFromBuffer( const achar* pBuffer );

	void SetShaderName( const achar* pShaderName );
	void AddVar( const CSMATMaterialVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	uint32 GetNumVars() const;
	const std::vector<CSMATMaterialVar>& GetVars() const;
	const achar* GetShaderName() const;

	void SetOutputDir( const achar* pOutputDir );
	const achar* GetOutputDir() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const;
	bool GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const;
	bool GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const;
	bool GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const;
	bool GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const;
	bool GrabValueAsString( const CJsonValue& jsonValue, std::string& value ) const;

	std::string						outputDir;
	std::string						shaderName;
	std::vector<CSMATMaterialVar>	vars;
};

#include "smatdoc/smat_source_doc.inl"