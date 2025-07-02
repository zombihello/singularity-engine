#ifndef SMAP_COMPILED_DOC_INL
#define SMAP_COMPILED_DOC_INL

/*
==================
CSMAPCompiledMapDoc::AddEntity
==================
*/
FORCEINLINE void CSMAPCompiledMapDoc::AddEntity( const CSMAPEntity& entity )
{
	entities.emplace_back( entity );
}

/*
==================
CSMAPCompiledMapDoc::RemoveEntity
==================
*/
FORCEINLINE void CSMAPCompiledMapDoc::RemoveEntity( uint32 index )
{
	Assert( index < entities.size() );
	entities.erase( entities.begin() + index );
}

/*
==================
CSMAPCompiledMapDoc::Clear
==================
*/
FORCEINLINE void CSMAPCompiledMapDoc::Clear()
{
	entities.clear();
}

/*
==================
CSMAPCompiledMapDoc::GetNumEntities
==================
*/
FORCEINLINE uint32 CSMAPCompiledMapDoc::GetNumEntities() const
{
	return ( uint32 )entities.size();
}

/*
==================
CSMAPCompiledMapDoc::GetEntities
==================
*/
FORCEINLINE const std::vector<CSMAPEntity>& CSMAPCompiledMapDoc::GetEntities() const
{
	return entities;
}

#endif // !SMAP_COMPILED_DOC_INL