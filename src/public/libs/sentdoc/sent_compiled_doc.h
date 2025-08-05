#pragma once
#include <string>
#include <vector>

#include "core/core.h"
#include "sentdoc/sent_component.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SENT (Singularity Entity Descriptor) files
//-----------------------------------------------------------------------------
class CSENTCompiledEntityDescDoc
{
public:
	// Save and load a entity descriptor document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool SaveFile( const achar* pPath );
	bool LoadFromFile( const achar* pPath );

	void AddComponent( const CSENTEntityDescComponent& component );
	void RemoveComponent( uint32 index );
	void Clear();

	uint32 GetNumComponents() const;
	const std::vector<CSENTEntityDescComponent>& GetComponents() const;

private:
	std::vector<CSENTEntityDescComponent>		components;
};

#include "sentdoc/sent_compiled_doc.inl"
