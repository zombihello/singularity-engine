#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "core/core.h"
#include "stdlib/jsondoc.h"
#include "utils/sentdoc/sent_component.h"

//-----------------------------------------------------------------------------
// Helper for work with source SENT (Singularity Entity Descriptor) files
//-----------------------------------------------------------------------------
class CSENTSourceEntityDescDoc
{
public:
	// Save and load a entity descriptor document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer)
	bool SaveFile( const char* pPath );
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer );

	void SetOutputDir( const char* pOutputDir );
	void AddComponent( const CSENTEntityDescComponent& component );
	void RemoveComponent( uint32 index );
	void Clear();

	const char*									   GetOutputDir() const;
	uint32										   GetNumComponents() const;
	const eastl::vector<CSENTEntityDescComponent>& GetComponents() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const;
	bool GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const;
	bool GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const;
	bool GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const;
	bool GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const;
	bool GrabValueAsString( const CJsonValue& jsonValue, eastl::string& value ) const;
	bool GrabValueAsComponent( const CJsonValue& jsonValue, CSENTEntityDescComponent& component ) const;
	bool GrabValueAsVar( const CJsonValue& jsonValue, CSENTEntityDescVar& var ) const;

	eastl::string							outputDir;
	eastl::vector<CSENTEntityDescComponent> components;
};

#include "utils/sentdoc/sent_source_doc.inl"
