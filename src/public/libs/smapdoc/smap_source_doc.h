#ifndef SMAP_SOURCE_DOC_H
#define SMAP_SOURCE_DOC_H

#include <vector>
#include <string>

#include "core/core.h"
#include "smapdoc/smap_entity.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMAP (Singularity Map) files
//-----------------------------------------------------------------------------
class CSMAPSourceMapDoc
{
public:
	// Load and save a file
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool LoadFromFile( const achar* pPath );
	bool SaveFile( const achar* pPath );

	void AddEntity( const CSMAPEntity& entity );
	void RemoveEntity( uint32 index );
	void Clear();

	const achar* GetOutputDir() const;
	uint32 GetNumEntities() const;
	const std::vector<CSMAPEntity>& GetEntities() const;

private:
	bool GrabData( const CJsonDoc& jsonDoc );
	bool GrabValueAsEntity( const CJsonValue& jsonValue, CSMAPEntity& entity ) const;

	std::string					outputDir;
	std::vector<CSMAPEntity>	entities;
};

#include "smapdoc/smap_source_doc.inl"

#endif // !SMAP_SOURCE_DOC_H
