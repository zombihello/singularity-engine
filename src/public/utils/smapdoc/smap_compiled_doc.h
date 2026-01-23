#pragma once
#include <EASTL/vector.h>

#include "tier0/tier0.h"
#include "utils/smapdoc/smap_entity.h"

//-----------------------------------------------------------------------------
// Helper for work with compiled SMAP (Singularity Map) files
//-----------------------------------------------------------------------------
class CSMAPCompiledMapDoc
{
public:
	// Load and save a file
	// NOTE: For use Tier1 must be connected by ConnectTier1
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );

	void AddEntity( const CSMAPEntity& entity );
	void RemoveEntity( uint32 index );
	void Clear();

	uint32							  GetNumEntities() const;
	const eastl::vector<CSMAPEntity>& GetEntities() const;

private:
	eastl::vector<CSMAPEntity> entities;
};

#include "utils/smapdoc/smap_compiled_doc.inl"
