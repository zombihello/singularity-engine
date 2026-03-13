#include "pch_game_shared.h"
#include "resourcesystem/iresourcesystem.h"
#include "game/shared/ecs/ecs_entitydesc.h"
#include "game/shared/ecs/ecs_entitydesc_factory.h"

/*
==================
CEcsEntityDescFactory::Init
==================
*/
void CEcsEntityDescFactory::Init()
{
	// Register the factory in the resource system
	Assert( g_pResourceSystem );
	flags |= GetDefaultFlags();
	g_pResourceSystem->RegisterResourceFactory( RESOURCE_TYPE_ENTITY_DESC, this );

	// Initialize the default entity descriptor
	pDefaultEntityDesc = g_pResourceSystem->FindOrLoadResource( "//CORE/entities/default", RESOURCE_TYPE_ENTITY_DESC );
	if ( !pDefaultEntityDesc )
	{
		pDefaultEntityDesc = g_pResourceSystem->CreateProceduralResource( "default", RESOURCE_TYPE_ENTITY_DESC );
	}
}

/*
==================
CEcsEntityDescFactory::Shutdown
==================
*/
void CEcsEntityDescFactory::Shutdown()
{
	// Unregister the factory from the resource system
	flags |= RESOURCE_FACTORY_FLAG_NOT_USED;
	g_pResourceSystem->UnRegisterResourceFactory( RESOURCE_TYPE_ENTITY_DESC );

	// Remove the default entity descriptor
	pDefaultEntityDesc = NULL;
}

/*
==================
CEcsEntityDescFactory::CreateProceduralResource
==================
*/
CRefPtr<IRefCounted> CEcsEntityDescFactory::CreateProceduralResource() const
{
	return new CEcsEntityDesc();
}

/*
==================
CEcsEntityDescFactory::LoadResource
==================
*/
CRefPtr<IRefCounted> CEcsEntityDescFactory::LoadResource( const char* pPath, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ ) const
{
	CSENTCompiledEntityDescDoc sentCompiledEntityDescDoc;
	if ( !sentCompiledEntityDescDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.sent_c", pPath ).c_str() ) )
	{
		return NULL;
	}

	return new CEcsEntityDesc( sentCompiledEntityDescDoc );
}

/*
==================
CEcsEntityDescFactory::UnloadResource
==================
*/
void CEcsEntityDescFactory::UnloadResource( IRefCounted* pResoruce ) const
{
	CEcsEntityDesc* pEntityDesc = (CEcsEntityDesc*)pResoruce;
	pEntityDesc->Clear();
}

/*
==================
CEcsEntityDescFactory::GetDefaultResource
==================
*/
CRefPtr<IResource> CEcsEntityDescFactory::GetDefaultResource() const
{
	return pDefaultEntityDesc;
}

/*
==================
CEcsEntityDescFactory::GetFormatType
==================
*/
const char* CEcsEntityDescFactory::GetFormatType() const
{
	return "SENT";
}
