#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "core/core.h"
#include "stdlib/jsondoc.h"
#include "utils/smatdoc/smatvar.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMAT (Singularity Material) files
//-----------------------------------------------------------------------------
class CSMATSourceMaterialDoc
{
public:
	// Save and load a material document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer)
	bool SaveFile( const char* pPath );
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer );

	void SetShaderName( const char* pShaderName );
	void AddVar( const CSMATMaterialVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	uint32								   GetNumVars() const;
	const eastl::vector<CSMATMaterialVar>& GetVars() const;
	const char*							   GetShaderName() const;

	void		SetOutputDir( const char* pOutputDir );
	const char* GetOutputDir() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const;
	bool GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const;
	bool GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const;
	bool GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const;
	bool GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const;
	bool GrabValueAsString( const CJsonValue& jsonValue, eastl::string& value ) const;

	eastl::string					outputDir;
	eastl::string					shaderName;
	eastl::vector<CSMATMaterialVar> vars;
};

#include "utils/smatdoc/smat_source_doc.inl"
