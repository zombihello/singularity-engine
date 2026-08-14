#include "pch_game_shared.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/ecs/ecs_entitydesc_loader.h"

/*
==================
CEcsEntityDescLoader::Load
==================
*/
bool CEcsEntityDescLoader::Load( const char* pPath, IResourceData* pData ) const
{
	PROFILER_SCOPE_FUNC();
	Assert( pData );
	CSENTCompiledEntityDescDoc sentCompiledEntityDescDoc;
	if ( !sentCompiledEntityDescDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.sent_c", pPath ).c_str() ) )
	{
		return false;
	}

	CEcsEntityDesc* pEcsEntityDesc = (CEcsEntityDesc*)pData;
	pEcsEntityDesc->Init( sentCompiledEntityDescDoc );
	return true;
}

/*
==================
CEcsEntityDescLoader::GetFormatName
==================
*/
const char* CEcsEntityDescLoader::GetFormatName() const
{
	return "Singularity Entity Descriptor";
}
