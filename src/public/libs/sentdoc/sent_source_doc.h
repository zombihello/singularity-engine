#pragma once
#include <string>
#include <vector>

#include "core/core.h"
#include "stdlib/jsondoc.h"
#include "sentdoc/sent_component.h"

//-----------------------------------------------------------------------------
// Helper for work with source SENT (Singularity Entity Descriptor) files
//-----------------------------------------------------------------------------
class CSENTSourceEntityDescDoc
{
public:
	// Save and load a entity descriptor document
	// NOTE: For use StdLib must be connected by ConnectStdLib (except LoadFromBuffer)
	bool SaveFile( const achar* pPath );
	bool LoadFromFile( const achar* pPath );
	bool LoadFromBuffer( const achar* pBuffer );

	void SetOutputDir( const achar* pOutputDir );
	void AddComponent( const CSENTEntityDescComponent& component );
	void RemoveComponent( uint32 index );
	void Clear();

	const achar* GetOutputDir() const;
	uint32 GetNumComponents() const;
	const std::vector<CSENTEntityDescComponent>& GetComponents() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const;
	bool GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const;
	bool GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const;
	bool GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const;
	bool GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const;
	bool GrabValueAsString( const CJsonValue& jsonValue, std::string& value ) const;
	bool GrabValueAsComponent( const CJsonValue& jsonValue, CSENTEntityDescComponent& component ) const;
	bool GrabValueAsVar( const CJsonValue& jsonValue, CSENTEntityDescVar& var ) const;

	std::string								outputDir;
	std::vector<CSENTEntityDescComponent>	components;
};

#include "sentdoc/sent_source_doc.inl"
