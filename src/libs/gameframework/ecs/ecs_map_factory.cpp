#include "pch_gameframework.h"
#include "resourcesystem/iresourcesystem.h"
#include "gameframework/ecs/ecs_map.h"
#include "gameframework/ecs/ecs_map_factory.h"

/*
==================
CEcsMapFactory::Init
==================
*/
void CEcsMapFactory::Init()
{
	// Register the factory in the resource system
	Assert( g_pResourceSystem );
	flags |= GetDefaultFlags();
	g_pResourceSystem->RegisterResourceFactory( RESOURCE_TYPE_MAP, this );

	// Initialize the default map
	pDefaultMap = g_pResourceSystem->FindOrLoadResource( "//CORE/maps/default", RESOURCE_TYPE_MAP );
	if ( !pDefaultMap )
	{
		pDefaultMap = g_pResourceSystem->CreateProceduralResource( "default", RESOURCE_TYPE_MAP );
	}
}

/*
==================
CEcsMapFactory::Shutdown
==================
*/
void CEcsMapFactory::Shutdown()
{
	// Unregister the factory from the resource system
	flags |= RESOURCE_FACTORY_FLAG_NOT_USED;
	g_pResourceSystem->UnRegisterResourceFactory( RESOURCE_TYPE_MAP );

	// Remove the default map
	pDefaultMap = NULL;
}

/*
==================
CEcsMapFactory::CreateProceduralResource
==================
*/
TRefPtr<IRefCounted> CEcsMapFactory::CreateProceduralResource() const
{
	return new CEcsMap();
}

/*
==================
CEcsMapFactory::LoadResource
==================
*/
TRefPtr<IRefCounted> CEcsMapFactory::LoadResource( const achar* pPath, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ ) const
{
	CSMAPCompiledMapDoc		smapCompiledMapDoc;
	if ( !smapCompiledMapDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smap_c", pPath ).c_str() ) )
	{
		return NULL;
	}

	return new CEcsMap( smapCompiledMapDoc );
}

/*
==================
CEcsMapFactory::UnloadResource
==================
*/
void CEcsMapFactory::UnloadResource( IRefCounted* pResoruce ) const
{
	CEcsMap*		pMap = ( CEcsMap* )pResoruce;
	pMap->Reset();
}

/*
==================
CEcsMapFactory::GetDefaultResource
==================
*/
TRefPtr<IResource> CEcsMapFactory::GetDefaultResource() const
{
	return pDefaultMap;
}

/*
==================
CEcsMapFactory::GetFormatType
==================
*/
const achar* CEcsMapFactory::GetFormatType() const
{
	return "SMAP";
}