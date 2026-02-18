#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/tier0.h"
#include "utils/sentdoc/sent_component.h"

//-----------------------------------------------------------------------------
// Helper for work with source SENT (Singularity Entity Descriptor) files
//-----------------------------------------------------------------------------
class CSENTSourceEntityDescDoc
{
public:
	// Save and load a entity descriptor document
	// NOTE: For use Tier1 must be connected by ConnectTier1 (except LoadFromBuffer)
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void SetOutputDir( const char* pOutputDir );
	void AddComponent( const CSENTEntityDescComponent& component );
	void RemoveComponent( uint32 index );

	const char*									   GetOutputDir() const;
	uint32										   GetNumComponents() const;
	const eastl::vector<CSENTEntityDescComponent>& GetComponents() const;

private:
	eastl::string							outputDir;
	eastl::vector<CSENTEntityDescComponent> components;
};

#include "utils/sentdoc/sent_source_doc.inl"
