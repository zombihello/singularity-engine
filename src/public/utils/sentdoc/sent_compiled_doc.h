#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "core/core.h"
#include "utils/sentdoc/sent_component.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SENT (Singularity Entity Descriptor) files
//-----------------------------------------------------------------------------
class CSENTCompiledEntityDescDoc
{
public:
	// Save and load a entity descriptor document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool SaveFile( const char* pPath );
	bool LoadFromFile( const char* pPath );

	void AddComponent( const CSENTEntityDescComponent& component );
	void RemoveComponent( uint32 index );
	void Clear();

	uint32										   GetNumComponents() const;
	const eastl::vector<CSENTEntityDescComponent>& GetComponents() const;

private:
	eastl::vector<CSENTEntityDescComponent> components;
};

#include "utils/sentdoc/sent_compiled_doc.inl"
