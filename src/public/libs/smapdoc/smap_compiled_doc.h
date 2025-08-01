#pragma once
#include <vector>

#include "core/core.h"
#include "smapdoc/smap_entity.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMAP (Singularity Map) files
//-----------------------------------------------------------------------------
class CSMAPCompiledMapDoc
{
public:
	// Load and save a file
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool LoadFromFile( const achar* pPath );
	bool SaveFile( const achar* pPath );

	void AddEntity( const CSMAPEntity& entity );
	void RemoveEntity( uint32 index );
	void Clear();

	uint32 GetNumEntities() const;
	const std::vector<CSMAPEntity>& GetEntities() const;

private:
	std::vector<CSMAPEntity>	entities;
};

#include "smapdoc/smap_compiled_doc.inl"
