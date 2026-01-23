#pragma once
#include <EASTL/vector.h>
#include <EASTL/string.h>

#include "tier0/tier0.h"
#include "utils/smapdoc/smap_entity.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMAP (Singularity Map) files
//-----------------------------------------------------------------------------
class CSMAPSourceMapDoc
{
public:
	// Load and save a file
	// NOTE: For use Tier1 must be connected by ConnectTier1
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );

	void AddEntity( const CSMAPEntity& entity );
	void RemoveEntity( uint32 index );
	void Clear();

	const char*						  GetOutputDir() const;
	uint32							  GetNumEntities() const;
	const eastl::vector<CSMAPEntity>& GetEntities() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsEntity( const CJsonValue& jsonValue, CSMAPEntity& entity ) const;

	eastl::string			   outputDir;
	eastl::vector<CSMAPEntity> entities;
};

#include "utils/smapdoc/smap_source_doc.inl"
