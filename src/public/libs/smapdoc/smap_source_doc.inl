#ifndef SMAP_SOURCE_DOC_INL
#define SMAP_SOURCE_DOC_INL

/*
==================
CSMAPSourceMapDoc::AddEntity
==================
*/
FORCEINLINE void CSMAPSourceMapDoc::AddEntity( const CSMAPEntity& entity )
{
	entities.emplace_back( entity );
}

/*
==================
CSMAPSourceMapDoc::RemoveEntity
==================
*/
FORCEINLINE void CSMAPSourceMapDoc::RemoveEntity( uint32 index )
{
	Assert( index < entities.size() );
	entities.erase( entities.begin() + index );
}

/*
==================
CSMAPSourceMapDoc::Clear
==================
*/
FORCEINLINE void CSMAPSourceMapDoc::Clear()
{
	entities.clear();
}

/*
==================
CSMAPSourceMapDoc::GetOutputDir
==================
*/
FORCEINLINE const achar* CSMAPSourceMapDoc::GetOutputDir() const
{
	return outputDir.c_str();
}

/*
==================
CSMAPSourceMapDoc::GetNumEntities
==================
*/
FORCEINLINE uint32 CSMAPSourceMapDoc::GetNumEntities() const
{
	return ( uint32 )entities.size();
}

/*
==================
CSMAPSourceMapDoc::GetEntities
==================
*/
FORCEINLINE const std::vector<CSMAPEntity>& CSMAPSourceMapDoc::GetEntities() const
{
	return entities;
}

#endif // !SMAP_SOURCE_DOC_INL